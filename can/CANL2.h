/*
 *   CAN Layer 2 API. Credits: Softing AG.
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

#ifndef _CANL2_H_
#define _CANL2_H_

#include <linux/types.h>

extern "C" {

#define OUT
#define CAN_HANDLE unsigned long
#define GET_FROM_SCIM -1

#define CANL2_SOCKET_AUTO 0xFFFF  // Socket automatically searched
#define CANL2_NO_IRQ 0x0          // no interrupt requested
#define CANL2_IRQ_AUTO 0xFFFF     // interrupt automatically assigned
#define CANL2_BASE_AUTO 0x0L      // DPR-Base automatically assigned
#define CANL2_SIZE_AUTO 0x0L      // DPR-Size automatically assigned
#define CANL2_AI_AUTO 0x0         // chip does no matter
#define CANL2_AI_DB8 0x01         // chip is Databook
#define CANL2_READ_INIFILE 0xFFFE // DOS CAN-AC2-104: reads INI-File

// CAN chip types
#define NEC005_CHIP 5
#define SJA1000_CHIP 1000

// Softing CAN Device Type
typedef enum {
    CANCARDNEC = 1,
    CANCARDSJA = 4,
    CANCARD2,
    CANPCI = 7,
    CANPCIDN,
    CAN_104,
    CANUSB,
    CAN104ECO = 12,
    CANPROPCIE,
    CAN104PLUS,
    CANPROUSB,
    EDICCARDC = 0x101,
    EDICCARD2 = 0x105,
    GEFANUCPC8 = 0x8000,
    FG100CAN = 0x10000000L
} DEVICETYPE;

#pragma pack(1) // pack alignment must be one

#ifndef CANL2_EXPORTS
#define MAXLENCHNAME 80
typedef struct
{
    __u32 u32Serial;     // serial number
    __u32 u32DeviceType; // device type
    __u32 u32PhysCh;     // physical channel number (1 or 2)
    __u32 u32FwId;       // id of the loaded firmware
    int bIsOpen;         // TRUE if opened by a process
    unsigned char ChannelName[MAXLENCHNAME];

} CHDSNAPSHOT, *PCHDSNAPSHOT;
#endif

typedef struct canl2_ch_s
{
    CAN_HANDLE ulChannelHandle;
    __u8 sChannelName[80];
} CANL2_CH_STRUCT;

typedef struct
{
    __u32 Ident;
    __s32 DataLength;
    __s32 RecOverrun_flag;
    __s32 RCV_fifo_lost_msg;
    __u8 RCV_data[8];
    __s32 AckOverrun_flag;
    __s32 XMT_ack_fifo_lost_acks;
    __s32 XMT_rmt_fifo_lost_remotes;
    __s32 Bus_state;
    __s32 Error_state;
    CAN_HANDLE Can;
    __u32 Time;
} PARAM_STRUCT;

typedef struct
{
    double fBaudrate;
    __s32 s32Prescaler;
    __s32 s32Tseg1;
    __s32 s32Tseg2;
    __s32 s32Sjw;
    __s32 s32Sam;
    __s32 s32AccCodeStd;
    __s32 s32AccMaskStd;
    __s32 s32AccCodeXtd;
    __s32 s32AccMaskXtd;
    __s32 s32OutputCtrl;
    __s8 bEnableAck;
    __s8 bEnableErrorframe;
    unsigned long hEvent;
} L2CONFIG;

#define INIPC_IB_ERR_VC_IOCANCELLED 0xFE05 // driver call cancelled
#define INIPC_IB_ERR_ACCESS_DPRAM 0xFFEA
#define CANPC_RB_ERR_DEV_LD -40 // CAN device not properly loaded

// ErrorCode = ErrorBase + ErrorDetail
// ErrorBase codes
#define CANPC_RB_BOOT_BASE -100 // boot-data errors
#define CANPC_RB_LOAD_BASE -200 // load-data errors
#define CANPC_RB_APP_BASE -300  // application-data errors
#define CANPC_RB_CPY_BASE -400  // copy-data errors
#define CANPC_RB_RUN_BASE -500  // run-data errors
// ErrorDetail codes
#define CANPC_RB_NO_RESPONSE -1 // no response from CAN-AC2
#define CANPC_RB_ERR_FMT -8     // boot-data format error
#define CANPC_RB_ERR_INT1 -2    // internal error 1
#define CANPC_RB_ERR_INT2 -3    // internal error 2
#define CANPC_RB_ERR_INT3 -4    // internal error 3
#define CANPC_RB_ERR_INT4 -9    // internal error 4
#define CANPC_RB_ERR_INT5 -10   // internal error 5
#define CANPC_RB_ERR_OPEN -5    // file open error (DOS only)
#define CANPC_RB_ERR_READ -6    // file read error (DOS only)
#define CANPC_RB_ERR_CLOSE -7   // file close error (DOS only)

#pragma pack() // reset pack alignment to default

#ifndef _ERROR_BASES_
#define _ERROR_BASES_
#define INFORMATIONAL_BASE (0x60000000)
#define WARNING_BASE (0xA0000000)
#define ERROR_BASE (0xE0000000)
#endif

#define CANL2_OK 0
#define CANL2_ERR -1
#define CANL2_BOARD_NOT_INITIALIZED -99
#define CANL2_INVALID_CHANNEL_HANDLE -1000 // the channel handle is invalid
#define CANL2_TOO_MANY_CHANNELS -1002      // no free channel available
#define CANL2_VERSION_CONFLICT -1003       // wrong dll or driver version
#define CANL2_FIRMWARE_DOWNLOAD                                                \
    -1004 // error while loading the firmware may be a DPRAM access error

#define CANL2_CAN_NOT_LOAD_CANUSBDLL                                           \
    -1005 // the canusbm.dll can not be found/loaded

#define INIL2_FW_FILE_NOT_FOUND -2000    // firmware file not found
#define INIL2_ERROR_WHILE_FLASHING -2001 // flashing error

#define FRW_ERROR_PIPEACCESS_ERROR -602    // unable to open usb pipe
#define FRW_ERROR_COMMUNICATIONBROKEN -603 // communication via usb pipe
#define FRW_ERR_NOVALID_LUPT_ENTRY -604    // no valid lookup table entry
#define FRW_IF_ERR_FRWINIT_FAILED -611 // CANusb Framewor initialization failed

#define CANL2_LIC_ERR -101
#define CANL2_WRONG_PARAM -102 // error in a parameter value
#define CANL2_FW_TIMEOUT -104  // timout while communicating with the firmware
#define CANL2_HW_ERR -108      // firmware runs on the wrong hardware
#define CANL2_DC_NO_DYN_OBJBUF -109 // no object dyn. buffer
#define CANL2_REQ_PENDING -110      // last request pending

#define CANL2_DATA_OVR -111        // receive data overrun
#define CANL2_REMOTE_OVR -112      // rec. rem. frame overrun
#define CANL2_NOT_ACTIVE -113      // object not active
#define CANL2_RXD_XMT_OVR -114     // transmit ack. overrun
#define CANL2_OB_NOT_DEFINED -115  // object not defined
#define CANL2_XMT_REQ_OVR -116     // xmt request fifo overrun
#define CANL2_NOT_IMPLEMENTED -117 // not supported for this device

// in this implementation the CAN handle is the same as the file descriptor
// for driver access; Please use the following macro for compatibility with
// later versions of this API (see sample program can_tst)
#define CANL2_handle_to_descriptor(handle) handle

__s32 INIL2_initialize_channel(CAN_HANDLE* pu32ChannelHandle,
                               char* pChannelName);

__s32 CANL2_reset_board(CAN_HANDLE Can);

__s32 CANL2_reset_chip(CAN_HANDLE Can);

__s32 CANL2_initialize_chip(CAN_HANDLE Can, __s32 presc, __s32 sjw, __s32 tseg1,
                            __s32 tseg2, __s32 sam);

__s32 CANL2_initialize_fifo_mode(CAN_HANDLE Can, L2CONFIG* pUserCfg);

__s32 CANL2_get_all_CAN_channels(__u32 u32ProvidedBufferSize,
                                 OUT __u32* pu32NeededBufferSize,
                                 OUT __u32* pu32NumOfChannels,
                                 OUT PCHDSNAPSHOT pBuffer);

// the parameter hw_version is not used with the CAN-AC2
__s32 CANL2_get_version(CAN_HANDLE Can, __s32* sw_version, __s32* fw_version,
                        __s32* hw_version, __s32* license,
                        __s32* can_chip_type);

// this function has no effect on the CAN-AC2
__s32 CANL2_get_serial_number(CAN_HANDLE Can, __u32* ser_number);

// -- CANusb return values
#define FRW_IOE_ERR_NOTENOUGHMEMORY -612 // not enough memory
#define FRW_IOE_ERR_INITFAILED -613      // failed to initialize FRWOrder/Event

__s32 CANL2_read_rcv_data(CAN_HANDLE Can, __s32 ObjectNumber, __u8* pRCV_Data,
                          __u32* Time);

#define CANL2_RA_NO_DATA 0             // no new data received
#define CANL2_RA_DATAFRAME 1           // std. data frame received
#define CANL2_RA_REMOTEFRAME 2         // std. remote frame received
#define CANL2_RA_TX_DATAFRAME 3        // transmission of std. data-
                                       //   frame is confirmed
#define CANL2_RA_TX_FIFO_OVR 4         // remote tx fifo overrun
#define CANL2_RA_CHG_BUS_STATE 5       // change of bus status
#define CANL2_RA_ERR_ADD 7             // additional error causes
#define CANL2_RA_TX_REMOTEFRAME 8      // transmission of std. data-
                                       //   frame is confirmed
#define CANL2_RA_XTD_DATAFRAME 9       // xtd. data frame received
#define CANL2_RA_XTD_TX_DATAFRAME 10   // transmission of xtd. data-
                                       //   frame is confirmed
#define CANL2_RA_XTD_TX_REMOTEFRAME 11 // transmission of xtd. remote-
                                       //   frame is confirmed
#define CANL2_RA_XTD_REMOTEFRAME 12    // xtd. remote frame received
                                       //   (only CANcard)
#define CANL2_RA_ERRORFRAME 15         // error frame detected
                                       //   (only CANcard)

__s32 CANL2_read_ac(CAN_HANDLE Can, PARAM_STRUCT* param);

__s32 CANL2_send_data(CAN_HANDLE Can, __u32 Ident, __s32 Xtd, __s32 DataLength,
                      __u8* pData);

__s32 CANL2_send_remote(CAN_HANDLE Can, __u32 Ident, __s32 Xtd,
                        __s32 DataLength);

__s32 CANL2_get_time(CAN_HANDLE Can, __u32* time);

#define CANL2_GBS_ERROR_ACTIVE 0  // error active
#define CANL2_GBS_ERROR_PASSIVE 1 // error passive
#define CANL2_GBS_ERROR_BUS_OFF 2 // bus off
__s32 CANL2_get_bus_state(CAN_HANDLE Can);

__s32 INIL2_close_channel(CAN_HANDLE Can);

// this function has no effect on the CAN-AC2
__s32 CANL2_enable_error_frame_detection(CAN_HANDLE Can);

__s32 CANL2_get_device_id(CAN_HANDLE Can, __u32* pulDeviceId);

} // extern "C"

#endif // __CANL2_H__
