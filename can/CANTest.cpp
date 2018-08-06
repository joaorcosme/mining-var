#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "canL2.h"
#include "CANproChannel.h"

// for time stamp calculation ...
__u32 Time, diff_time, old_time;

__u32 Identifier = 0; // Identifier

bool stop_thread1 = false;

char *FormatData(char *buffer, unsigned char *pData, long dataLen) {
    int i;

    memset(buffer, 0, 25);

    for (i = 0; i < 8; i++) {
        if (i < dataLen) {
            sprintf(&buffer[i * 3], "%3x", pData[i]);
        } else {
            strcat(buffer, "   ");
        }
    }

    return buffer;
}

//    ReadBusEventFIFO
//    Reads the event data of a received event
int ReadBusEvent(CAN_HANDLE can, int ch, unsigned long *diff_time,
                 unsigned long *old_time) {
    char buffer[25];

    PARAM_STRUCT param;
    param.DataLength = 3;

    int frc = CANL2_read_ac(can, &param);

    switch (frc) {
    case CANL2_RA_DATAFRAME:

        *diff_time = param.Time - *old_time;
        printf("RCV STD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", ch,
               param.Ident, param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               *diff_time);
        *old_time = param.Time;

        break;
    case CANL2_RA_REMOTEFRAME:
        *diff_time = param.Time - *old_time;
        printf("RCV STD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               ch, param.Ident, param.DataLength, param.Time, *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_XTD_DATAFRAME: // not valid with AC2 with 82C200
        *diff_time = param.Time - *old_time;
        printf("RCV XTD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", ch,
               param.Ident, param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_XTD_REMOTEFRAME: // not valid with AC2 with 82C200
        *diff_time = param.Time - *old_time;
        printf("RCV XTD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               ch, param.Ident, param.DataLength, param.Time, *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_TX_DATAFRAME:
        *diff_time = param.Time - *old_time;

        printf("ACK STD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", ch,
               param.Ident, param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_TX_REMOTEFRAME:
        *diff_time = param.Time - *old_time;
        printf("ACK STD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               ch, param.Ident, param.DataLength, param.Time, *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_XTD_TX_DATAFRAME: // not valid with AC2 with 82C200
        *diff_time = param.Time - *old_time;
        printf("ACK XTD CAN%u Id%8x Dlc%u Data%s T%8x D%10lu\n", ch,
               param.Ident, param.DataLength,
               FormatData(buffer, param.RCV_data, param.DataLength), param.Time,
               *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_XTD_TX_REMOTEFRAME: // not valid with AC2 with 82C200
        *diff_time = param.Time - *old_time;
        printf("ACK XTD CAN%u Id%8x Dlc%u REMOTE                       "
               "T%8x D%10lu\n",
               ch, param.Ident, param.DataLength, param.Time, *diff_time);
        *old_time = param.Time;
        break;
    case CANL2_RA_CHG_BUS_STATE:
        *diff_time = param.Time - *old_time;
        printf("ERROR  CAN%u Bus state: %1d                      T%8x D%10lu\n",
               ch, param.Bus_state, param.Time, *diff_time);
        *old_time = param.Time;
        break;
    default:
        break;
    }
    return (frc);
}

void interruptionThread(const CAN_HANDLE &can_channel) {
    unsigned long diff_time = 0, old_time = 0;
    int ret, RetCode;

    struct pollfd can_poll;

    while (!stop_thread1) {
        // in this implementation the CAN handle is the same as the file
        // descriptor
        // for driver access; Please use the following macro
        // CANL2_handle_to_descriptor
        // for compatibility with later versions of this API

        can_poll.fd = CANL2_handle_to_descriptor(can_channel);
        can_poll.events = POLLIN | POLLHUP;

        do {
            ret = poll(&can_poll, 1, -1);
            //  printf("interruptionThread: poll returned %u, events=0x%x.
            //  revents=0x%x,
            //  POLLHUP=0x%x\n",ret,can_poll.events,can_poll.revents,POLLHUP);

            if (can_poll.revents & POLLHUP) {
                printf("terminating interruptionThread()\n");
                exit(0);
                return;
            }

            if ((ret == -1) && (errno != EINTR)) {
                printf("Error: poll \"\" failed: (%d) %s\n", errno,
                       strerror(errno));
            }
        } while (ret <= 0);

        do {
            RetCode = ReadBusEvent(can_channel, 1, &diff_time, &old_time);
            if (stop_thread1)
                return;
        } while (RetCode > 0);

        if (RetCode < 0)
            stop_thread1 = true;
    }

    printf("terminating interruptionThread\n");
    return;
}

// handle the input from the user
void UserRequestFIFO(char option, CAN_HANDLE can1) {

    static unsigned char Xtd = 0;
    int DataLength = 3;          // Datalength of the data frames
    unsigned char p_XMT_data[8]; // Array of data bytes to be sent
    int frc;                     // Function return code
    char buffer[25];
    int k = 0, i; // Loop variables

    switch (option) {
    case 'h': // Output of the help menu
        printf("\n ----- COMMUNICATION KEYS\n");
        printf("t: transmit data can1\n");
        printf("r: transmit remote can1\n");
        printf("\n -----  ADMINISTRATION KEYS\n");
        printf("l: get time\n");
        printf("k: reset fifos\n");
        printf("G: get fifo levels\n");
        printf("L: reset lost msg counters\n");
        printf("B: get bus state can1\n");
        printf("h: help menu\nq: quit\n");
        break;

    case 't': // Send data frame on CAN 1

        Identifier = (Identifier > 7) ? 0 : Identifier + 1;
        for (i = 0; i <= 7; i++)
            p_XMT_data[i] = (unsigned char)(Identifier + i + k++);
        DataLength = (int)(8 - Identifier);

        frc = CANL2_send_data(can1, Identifier, Xtd, DataLength, p_XMT_data);
        printf("XMT     CAN%u Id%8x Dlc%u Data%s return Code%2d\n", 1,
               Identifier, DataLength,
               FormatData(buffer, p_XMT_data, DataLength), frc);
        break;

    case 'r': // Send remote frame on CAN 1
        Identifier = (Identifier > 7) ? 0 : Identifier + 1;
        DataLength = (DataLength + 1) & 0x7;

        frc = CANL2_send_remote(can1, Identifier, Xtd, DataLength);
        printf("XMT     CAN%u Id%8x Dlc%u REMOTE                      "
               "return Code%2d\n",
               1, Identifier, DataLength, frc);
        break;

    case 'l':
        CANL2_get_time(can1, &Time);
        diff_time = Time - old_time;
        printf("time:                                                    "
               "T%8x D%10u\n",
               Time, diff_time);
        old_time = Time;
        break;

    case 'k':
        CANL2_reset_rcv_fifo(can1);
        CANL2_reset_xmt_fifo(can1);
        printf("receive fifos are empty now!\n");
        break;

    case 'L':
        CANL2_reset_lost_msg_counter(can1);
        printf("lost message counters cleared\n");
        break;

    case 'G': // Read transmit and receive fifos and the lost messages
        // counter
        printf("CAN%u rcv fifo level:%d xmt fifo level:%d\n", 1,
               CANL2_read_rcv_fifo_level(can1),
               CANL2_read_xmt_fifo_level(can1));
        break;

    case 'B':
        printf("bus state CAN%u:%d \n", 1, CANL2_get_bus_state(can1));
        break;

    default:
        break;
    }
}

int main(int argc, char **argv) {

    try {
        CANproChannel channel;
        std::cout << "#INFO: The CANpro channel is online now." << std::endl;

        std::thread interruptionHandler(interruptionThread, channel.getHandle());

        // TODO: still needs refactoring (begin)
        char option = 'h';
        while (option != 'q') {
            UserRequestFIFO(option, channel.getHandle());
            scanf("%c", &option);
        }

        stop_thread1 = true;

        CANL2_reset_chip(channel.getHandle());

        interruptionHandler.join();

        INIL2_close_channel(channel.getHandle());
        // TODO: still needs refactoring (end)

    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
