#include <cerrno>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

#include "CANproChannel.h"
#include "canL2.h"

#define DEBUG_INTERRUPTION(MSG)                                                \
    if (0)                                                                     \
    std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

// for time stamp calculation ...
__u32 Time, diff_time, old_time;

static char *FormatData(char *buffer, unsigned char *pData, long dataLen) {
    memset(buffer, 0, 25);

    for (int i = 0; i < 8; ++i) {
        if (i < dataLen) {
            sprintf(&buffer[i * 3], "%3x", pData[i]);
        } else {
            strcat(buffer, "   ");
        }
    }

    return buffer;
}

static int readBusEvent(CAN_HANDLE can) {
    char buffer[25];

    PARAM_STRUCT param;
    param.DataLength = 3;

    int frc = CANL2_read_ac(can, &param);
    switch (frc) {
    case CANL2_RA_DATAFRAME:

        diff_time = param.Time - old_time;
        printf("RCV STD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", 1, param.Ident,
               param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               diff_time);
        old_time = param.Time;

        break;
    case CANL2_RA_REMOTEFRAME:
        diff_time = param.Time - old_time;
        printf("RCV STD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               1, param.Ident, param.DataLength, param.Time, diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_XTD_DATAFRAME: // not valid with AC2 with 82C200
        diff_time = param.Time - old_time;
        printf("RCV XTD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", 1, param.Ident,
               param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_XTD_REMOTEFRAME: // not valid with AC2 with 82C200
        diff_time = param.Time - old_time;
        printf("RCV XTD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               1, param.Ident, param.DataLength, param.Time, diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_TX_DATAFRAME:
        diff_time = param.Time - old_time;

        printf("ACK STD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", 1, param.Ident,
               param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_TX_REMOTEFRAME:
        diff_time = param.Time - old_time;
        printf("ACK STD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               1, param.Ident, param.DataLength, param.Time, diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_XTD_TX_DATAFRAME: // not valid with AC2 with 82C200
        diff_time = param.Time - old_time;
        printf("ACK XTD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", 1, param.Ident,
               param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_XTD_TX_REMOTEFRAME: // not valid with AC2 with 82C200
        diff_time = param.Time - old_time;
        printf("ACK XTD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               1, param.Ident, param.DataLength, param.Time, diff_time);
        old_time = param.Time;
        break;
    case CANL2_RA_CHG_BUS_STATE:
        diff_time = param.Time - old_time;
        printf("ERROR  CAN%u Bus state: %1d                      T%8x D%10lu\n",
               1, param.Bus_state, param.Time, diff_time);
        old_time = param.Time;
        break;
    default:
        break;
    }
    return frc;
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
        while ((ret = readBusEvent(channel))) {
            if (ret < 0 || shouldTerminate(futureSignal)) {
                goto endthread;
            }
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
