#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#include <cassert>
#include <cerrno>

#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include "CANproChannel.h"
#include "canL2.h"

#define DEBUG_INTERRUPTION(MSG)                                                \
    if (0)                                                                     \
    std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

constexpr char hexMap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static std::string hexStr(const __u8 *data, const __s32 len) {
    std::string hexData(24, ' ');
    for (unsigned i = 0; i < len; ++i) {
        hexData[3 * i] = hexMap[(data[i] & 0xF0) >> 4];
        hexData[3 * i + 1] = hexMap[data[i] & 0x0F];
    }
    return hexData;
}

static void printReceivedData(int frc, const PARAM_STRUCT &param) {
    // for now, we are only interested in this type of frame
    if (frc == CANL2_RA_DATAFRAME) {
        std::cout << "RCV STD CAN1 :::: "
                  << "ID " << std::hex << std::setfill(' ') << std::setw(5)
                  << param.Ident << " :: LEN " << param.DataLength
                  << " :: DATA " << hexStr(param.RCV_data, param.DataLength)
                  << std::endl;
    } else {
        assert(false);
    }
}

static int readBusEvent(CAN_HANDLE can, PARAM_STRUCT &retParam) {
    retParam.DataLength = 3;
    return CANL2_read_ac(can, &retParam);
}

static bool shouldTerminate(const std::future<void> &signal) {
    return signal.wait_for(std::chrono::system_clock::duration::zero()) ==
           std::future_status::ready;
}

static void interruption(CAN_HANDLE channel, std::future<void> futureSignal) {
    DEBUG_INTERRUPTION("Thread start");

    struct pollfd can_poll;

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
                std::cerr << "#Error: poll() [" << strerror(errno) << "]";
            }
        }

        DEBUG_INTERRUPTION("Read section");
        // desciptor is ready to be read
        PARAM_STRUCT outParam;
        while ((ret = readBusEvent(channel, outParam))) {
            if (ret < 0 || shouldTerminate(futureSignal)) {
                goto endthread;
            }
            printReceivedData(ret, outParam);
        }
    }

endthread:
    DEBUG_INTERRUPTION("Thread end");
    return;
}

int main(int argc, char **argv) {

    try {
        can::CANproChannel channel;
        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread interruptionHandler(interruption, channel.getHandle(),
                                        std::move(futureSignal));
        std::getchar();

        exitSignal.set_value();
        CANL2_reset_chip(channel.getHandle());

        interruptionHandler.join();

    } catch (std::runtime_error &ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
