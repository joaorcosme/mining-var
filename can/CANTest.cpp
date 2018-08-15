/*
 *   Displays the data transmitted by the BS-9000 sensor in the CAN bus
 *   for testing purposes.
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

#include "CANproChannel.h"
#include "BSFrameHandler.h"
#include "CANL2.h"
#include "CANUtils.h"
#include "CANproChannel.h"
#include "DetectionGUI.h"

#include <cerrno>
#include <cstring>
#include <sys/poll.h>

#include <chrono>
#include <experimental/optional>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

#define DEBUG_INTERRUPTION(MSG)                                                \
    if (false)                                                                 \
    std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

#define DEBUG_RECV_DATA false

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

namespace can {

static void interruption(CAN_HANDLE channel, backsense::RadarStateDB& stateDB,
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

} // namespace can

int main(int argc, char** argv)
{

    try {
        can::CANproChannel channel;
        can::backsense::RadarStateDB stateDB(1);
        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread interruptionHandler(can::interruption, channel.getHandle(),
                                        std::ref(stateDB),
                                        std::move(futureSignal));

        gui::DetectionGUI interface(stateDB);
        interface.launchGUI(); // blocking call

        exitSignal.set_value();

        can::CANUtils::resetChip(channel.getHandle());
        interruptionHandler.join();

    } catch (std::runtime_error& ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
