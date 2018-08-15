/*
 *   Helper functions to handle CAN bus operations.
 *
 *   Copyright (C) 2018  Joao Cosme <joaorcosme@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "CANUtils.h"
#include "BSFrameHandler.h"
#include "CANproChannel.h"

#include <cassert>
#include <cerrno>
#include <cstring>
#include <sys/poll.h>

#include <chrono>
#include <experimental/optional>
#include <iomanip>
#include <iostream>
#include <sstream>

#define DEBUG_INTERRUPTION(MSG)                                                \
    if (false)                                                                 \
    std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

#define DEBUG_RECV_DATA false

using can::CANUtils;

constexpr std::array<char, 16> CANUtils::m_hexMap;

int CANUtils::readBusEvent(CAN_HANDLE can, PARAM_STRUCT& retParam)
{
    retParam.DataLength = 3;
    return CANL2_read_ac(can, &retParam);
}

std::string CANUtils::formatHexStr(const __u8* data, const __s32 len)
{
    // input: an array of bytes, like: |100|045|099|000|253|022|009|150|
    // each byte is a __u8 type, which corresponds to a char
    //
    // output: a string where each byte is represented in its hexadecimal
    // representation

    constexpr unsigned MAXLEN = 8;
    assert(len <= MAXLEN);

    // each byte will take 3 positions in the string, like:
    // "64 2D 63 00 FD 16 09 96"
    std::string hexData(3 * MAXLEN, ' ');
    for (unsigned i = 0; i < len; ++i) {
        hexData[3 * i] = m_hexMap[(data[i] & 0xF0) >> 4];
        hexData[3 * i + 1] = m_hexMap[data[i] & 0x0F];
    }
    return hexData;
}

void CANUtils::printReceivedData(int frc, const PARAM_STRUCT& param)
{
    // for now, we are only interested in this type of frame
    if (frc == CANL2_RA_DATAFRAME) {
        std::cout << "RCV STD CAN1 :::: "
                  << "ID " << std::hex << std::setfill(' ') << std::setw(5)
                  << param.Ident << " :: LEN " << param.DataLength
                  << " :: DATA "
                  << formatHexStr(param.RCV_data, param.DataLength)
                  << std::endl;
    } else {
        assert(false);
    }
}

static bool shouldTerminate(const std::future<void>& signal)
{
    return signal.wait_for(std::chrono::system_clock::duration::zero()) ==
           std::future_status::ready;
}

static void printDetectionData(const can::backsense::DetectionData& state)
{
    std::ostringstream ss;
    state.dump(ss);
    std::cout << ss.str();
}

void CANUtils::interruption(CAN_HANDLE channel,
                            backsense::RadarStateDB& stateDB,
                            std::future<void> futureSignal)
{
    DEBUG_INTERRUPTION("Thread start");

    struct pollfd can_poll;

    backsense::FrameHandler frameHandler;

    while (!shouldTerminate(futureSignal)) {

        can_poll.fd = CANL2_handle_to_descriptor(channel);
        can_poll.events = POLLIN | POLLHUP;

        int ret = 0;

        DEBUG_INTERRUPTION("Poll section");
        // wait for event on file descriptor
        while (ret <= 0) {
            ret = poll(&can_poll, 1 /*nfds*/, -1 /*timeout*/);

            if (can_poll.revents & POLLHUP) {
                goto endthread;
            }
            if ((ret == -1) && (errno != EINTR)) {
                std::cerr << "#Error: poll() [" << std::strerror(errno) << "]";
            }
        }

        DEBUG_INTERRUPTION("Read section");
        // descriptor is ready to be read
        PARAM_STRUCT outParam;
        while ((ret = CANUtils::readBusEvent(channel, outParam))) {
            if (ret < 0 || shouldTerminate(futureSignal)) {
                goto endthread;
            }

            if (DEBUG_RECV_DATA) {
                CANUtils::printReceivedData(ret, outParam);
            }

            auto state = frameHandler.processRcvFrame(outParam);
            if (state) {
                if (DEBUG_RECV_DATA) {
                    printDetectionData(*state);
                }

                // This is probably the most important step in this loop:
                // we've read the raw data from the CAN bus, converted into
                // a DetectionData object, and now we are able to update the DB,
                // overwriting the state for the corresponding object id.
                stateDB.updateState(std::move(*state));
            }
        }
    }

endthread:
    DEBUG_INTERRUPTION("Thread end");
    return;
}
