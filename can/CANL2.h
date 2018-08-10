// Based on the CANL2 interface provided by Softing

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
typedef struct {
    __u32 u32Serial;     // serial number
    __u32 u32DeviceType; // device type
    __u32 u32PhysCh;     // physical channel number (1 or 2)
    __u32 u32FwId;       // id of the loaded firmware
    int bIsOpen;         // TRUE if opened by a process
    unsigned char ChannelName[MAXLENCHNAME];

} CHDSNAPSHOT, *PCHDSNAPSHOT;
#endif

typedef struct canl2_ch_s {
    CAN_HANDLE ulChannelHandle;
    __u8 sChannelName[80];
} CANL2_CH_STRUCT;

typedef struct {
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

typedef struct {
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

#define L2_ERROR_BASE (0x000B0000)
#define VCARD_ERROR_BASE (0x000A0000)

#define MK_VCARD_ERROR_CODE(VCARD_ERROR)                                       \
    ((VCARD_ERROR) ? (VCARD_ERROR_BASE | ERROR_BASE | VCARD_ERROR) : 0)

//  VCARD ERROR CODES (Error codes from kernelmode driver)
#ifndef CANL2_EXPORTS

#define VCARD_OK (__u32)0                     // success
#define VCARD_INTERNALERROR (__u32)0xE00A0001 // internal Error
#define VCARD_GENERALERROR (__u32)0xE00A0002  // general Error
#define VCARD_TIMEOUT (__u32)0xE00A0003       // Timeout
#define VCARD_IOPENDING (__u32)0xE00A0004     // pending driver call
#define VCARD_IOCANCELLED (__u32)0xE00A0005   // driver call was canceled
#define VCARD_ILLEGALCALL (__u32)0xE00A0006   // illegal driver call
#define VCARD_NOTSUPPORTED (__u32)0xE00A0007  // not implemented
#define VCARD_VERSIONERROR                                                     \
    (__u32)0xE00A0008 // driver interface dll has the wrong version
#define VCARD_DRIVERVERSIONERROR (__u32)0xE00A0009 // wrong driver version
#define VCARD_DRIVERNOTFOUND                                                   \
    (__u32)0xE00A000A // driver not loaded / not installed, or device is not
                      // plugged
#define VCARD_NOTENOUGHMEMORY (__u32)0xE00A000B // out of memory
#define VCARD_TOOMANYDEVICES                                                   \
    (__u32)0xE00A000C // driver can not handle as much devices
#define VCARD_UNKNOWNDEVICE (__u32)0xE00A000D       // unknown device
#define VCARD_DEVICEALREADYEXISTS (__u32)0xE00A000E // device already exists
#define VCARD_DEVICEACCESSERROR                                                \
    (__u32)0xE00A000F // access is not possible: device is already open
#define VCARD_RESOURCEALREADYREGISTERED                                        \
    (__u32)0xE00A0010 // resource is in use by an other device
#define VCARD_RESOURCECONFLICT (__u32)0xE00A0011 // resource conflict
#define VCARD_RESOURCEACCESSERROR                                              \
    (__u32)0xE00A0012 // resource can not be accessed
#define VCARD_PHYSMEMORYOVERRUN                                                \
    (__u32)0xE00A0013 // ungueltiger Zugriff auf physikalischen Speicher
#define VCARD_TOOMANYPORTS (__u32)0xE00A0014 // zu viele I/O Ports 20
#define VCARD_INTERRUPTERROR                                                   \
    (__u32)0xE00A0015 // error while activating the interrupt
#define VCARD_UNKNOWNRESOURCE (__u32)0xE00A0016 // unknown ressorce
#define VCARD_CREATEDEVFAILED (__u32)0xE00A0017 // IoCreateDevice failed
#define VCARD_ATTACHTODEVSTACKFAILED                                           \
    (__u32)0xE00A0018 // IoAttachDeviceToDeviceStack failed
#define VCARD_SYMBOLICLINKFAILED                                               \
    (__u32)0xE00A0019 // failed to create a symbolic link

//    Errors which can occur while downloading the firmware
#define VCARD_NOCARDSERVICES                                                   \
    (__u32)0xE00A001A // can not access card services under Win '98
#define VCARD_CARDSERVICESVERSION                                              \
    (__u32)0xE00A001B // wrong version of the card services under Win '98
#define VCARD_CARDSERVICESGETINFO                                              \
    (__u32)0xE00A001C // error while accessing the card services under Win '98
#define VCARD_DEVICENOTFOUND (__u32)0xE00A001D // device not found.
#define VCARD_NODPRAM                                                          \
    (__u32)0xE00A001E // can not get a free address region for DPRAM from
                      // system
#define VCARD_CONTROLHWERROR (__u32)0xE00A001F // Error while accessing hardware
#define VCARD_SBNCHECKSUM                                                      \
    (__u32)0xE00A0020 // Checksum error in SBN format (dll binary may be
                      // corrupt)
#define VCARD_DPRAMACCESS (__u32)0xE00A0021 // can not access the DPRAM memory
#define VCARD_CARDREACTION                                                     \
    (__u32)0xE00A0022 // Loader program for firmware download does no more
                      // react.
#define VCARD_NOSTARTADDRESS                                                   \
    (__u32)0xE00A0023 // No startaddress defined in SBN (dll binary may be
                      // corrupt)
#define VCARD_NOINTERRUPT (__u32)0xE00A0024 // Interrupt does not work

//    Errors which can occur in the channel driver
#define VCARD_DRIVERNOTPRESENT                                                 \
    (__u32)0xE00A0025                        // Kernel mode driver is not loaded
#define VCARD_DEVICEISOPEN (__u32)0xE00A0026 // Device is already open
#define VCARD_DEVICELOCKINGERROR (__u32)0xE00A0027 // Device can not be locked
#define VCARD_OTHERFWISLOADED                                                  \
    (__u32)0xE00A0028 // A other firmware is running on that device
                      // (CANalyzer/CANopen/DeviceNet firmware)
#define VCARD_CHANNELNOTOPEN                                                   \
    (__u32)0xE00A0029 // channel can not be accessed, because it is not open.

//    Status codes for CANusb Device Driver canusbw.sys
#define VCARD_PNPCALLERROR                                                     \
    (__u32)0xE00A002A // lower driver call in PnP process fails
#define VCARD_URBRETERROR (__u32)0xE00A002B // URB returns USBD_ERROR code
#define VCARD_ERRORDEVICEDESC                                                  \
    (__u32)0xE00A002C // faulty device desc or read failed
#define VCARD_ERRORCONFIGDESC                                                  \
    (__u32)0xE00A002D // faulty config desc or read failed
#define VCARD_ERRORSELECTCONFIG                                                \
    (__u32)0xE00A002E // unable to select configuration
#define VCARD_ERRORDECONFIG                                                    \
    (__u32)0xE00A002F // unable to deconfigure the device
#define VCARD_PIPEACCESSERROR (__u32)0xE00A0030 // unable to open usb pipe
#define VCARD_COMMUNICATIONBROKEN                                              \
    (__u32)0xE00A0031 // communication via usb pipe broken off

//    Errors which can occur in the canchd.dll
#define VCARD_FILENOTFOUND (__u32)0xE00A0032 // file not found
#define VCARD_ACCESSRIGHT (__u32)0xE00A0033

#define VCARD_OSERROR (__u32)0xE00A0034 // error in OS call
#define VCARD_DEVICEIDMISMATCH                                                 \
    (__u32)0xE00A0035 // wrong device id stored in registry
#define VCARD_MAXNUMOFCHANNELSOPEN                                             \
    (__u32)0xE00A0036 // the maximum number of channels are open
#define VCARD_INVALIDHANDLE (__u32)0xE00A0037 // a invalid handle is specified

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

__s32 INIL2_initialize_channel(CAN_HANDLE *pu32ChannelHandle,
                               char *pChannelName);

__s32 CANL2_reset_board(CAN_HANDLE Can);

__s32 CANL2_reset_chip(CAN_HANDLE Can);

__s32 CANL2_initialize_chip(CAN_HANDLE Can, __s32 presc, __s32 sjw, __s32 tseg1,
                            __s32 tseg2, __s32 sam);

__s32 CANL2_set_mode(CAN_HANDLE Can, __s32 SleepMode, __s32 SpeedMode);

__s32 CANL2_set_acceptance(CAN_HANDLE Can, __u32 AccCodeStd, __u32 AccMaskStd,
                           __u32 AccCodeXtd, __u32 AccMaskXtd);

__s32 CANL2_set_output_control(CAN_HANDLE Can, __s32 OutputControl);

__s32 CANL2_initialize_interface(
    CAN_HANDLE Can, __s32 ReceiveFifoEnable, __s32 ReceivePollAll,
    __s32 ReceiveEnableAll, __s32 ReceiveIntEnableAll, __s32 AutoRemoteEnable,
    __s32 TransmitReqFifoEnable, __s32 TransmitPollAll,
    __s32 TransmitAckEnableAll, __s32 TransmitAckFifoEnable,
    __s32 TransmitRmtFifoEnable);

__s32 CANL2_define_object(CAN_HANDLE Can, __u32 Handle, __s32 *ObjectNumber,
                          __s32 Type, __s32 ReceiveIntEnable,
                          __s32 AutoRemoteEnable, __s32 TransmitAckEnable);

__s32 CANL2_define_cyclic(CAN_HANDLE Can, __s32 ObjectNumber, __u32 Rate,
                          __u32 Cycles);

__s32 CANL2_set_rcv_fifo_size(CAN_HANDLE Can, __s32 FifoSize);

#define CANL2_WRONG_MODE -1001
__s32 CANL2_enable_fifo(CAN_HANDLE Can);

// this function has no effect on the CANcard
__s32 CANL2_optimize_rcv_speed(CAN_HANDLE Can);

__s32 CANL2_enable_dyn_obj_buf(CAN_HANDLE Can);

__s32 CANL2_enable_fifo_transmit_ack(CAN_HANDLE Can);

__s32 CANL2_initialize_fifo_mode(CAN_HANDLE Can, L2CONFIG *pUserCfg);

__s32 CANL2_get_all_CAN_channels(__u32 u32ProvidedBufferSize,
                                 OUT __u32 *pu32NeededBufferSize,
                                 OUT __u32 *pu32NumOfChannels,
                                 OUT PCHDSNAPSHOT pBuffer);

// the parameter hw_version is not used with the CAN-AC2
__s32 CANL2_get_version(CAN_HANDLE Can, __s32 *sw_version, __s32 *fw_version,
                        __s32 *hw_version, __s32 *license,
                        __s32 *can_chip_type);

// this function has no effect on the CAN-AC2
__s32 CANL2_get_serial_number(CAN_HANDLE Can, __u32 *ser_number);

// -- CANusb return values
#define FRW_IOE_ERR_NOTENOUGHMEMORY -612 // not enough memory
#define FRW_IOE_ERR_INITFAILED -613      // failed to initialize FRWOrder/Event

__s32 CANL2_start_chip(CAN_HANDLE Can);

__s32 CANL2_send_remote_object(CAN_HANDLE Can, __s32 ObjectNumber,
                               __s32 DataLength);

__s32 CANL2_supply_object_data(CAN_HANDLE Can, __s32 ObjectNumber,
                               __s32 DataLength, __u8 *pData);

__s32 CANL2_supply_rcv_object_data(CAN_HANDLE Can, __s32 ObjectNumber,
                                   __s32 DataLength, __u8 *pData);

__s32 CANL2_send_object(CAN_HANDLE Can, __s32 ObjectNumber, __s32 DataLength);

__s32 CANL2_write_object(CAN_HANDLE Can, __s32 ObjectNumber, __s32 DataLength,
                         __u8 *pData);

__s32 CANL2_read_rcv_data(CAN_HANDLE Can, __s32 ObjectNumber, __u8 *pRCV_Data,
                          __u32 *Time);

__s32 CANL2_read_xmt_data(CAN_HANDLE Can, __s32 ObjectNumber,
                          __s32 *pDataLength, __u8 *pXMT_Data);

// +--- CANL2_read_ac                ---------------------------------------*/
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

__s32 CANL2_read_ac(CAN_HANDLE Can, PARAM_STRUCT *param);

__s32 CANL2_send_data(CAN_HANDLE Can, __u32 Ident, __s32 Xtd, __s32 DataLength,
                      __u8 *pData);

__s32 CANL2_send_remote(CAN_HANDLE Can, __u32 Ident, __s32 Xtd,
                        __s32 DataLength);

// these functions have no effect on the CANcard
__s32 CANL2_get_trigger(CAN_HANDLE Can, __s32 *level);

__s32 CANL2_reinitialize(CAN_HANDLE Can);

__s32 CANL2_get_time(CAN_HANDLE Can, __u32 *time);

#define CANL2_GBS_ERROR_ACTIVE 0  // error active
#define CANL2_GBS_ERROR_PASSIVE 1 // error passive
#define CANL2_GBS_ERROR_BUS_OFF 2 // bus off
__s32 CANL2_get_bus_state(CAN_HANDLE Can);

__s32 CANL2_reset_rcv_fifo(CAN_HANDLE Can);

__s32 CANL2_reset_xmt_fifo(CAN_HANDLE Can);

__s32 CANL2_reset_lost_msg_counter(CAN_HANDLE Can);

// returns number of items in receive fifo
__s32 CANL2_read_rcv_fifo_level(CAN_HANDLE Can);

// returns number of items in transmit fifo
__s32 CANL2_read_xmt_fifo_level(CAN_HANDLE Can);

__s32 INIL2_close_channel(CAN_HANDLE Can);

// this function has no effect on the CAN-AC2
__s32 CANL2_enable_error_frame_detection(CAN_HANDLE Can);

__s32 CANL2_get_device_id(CAN_HANDLE Can, __u32 *pulDeviceId);

/* these functions are only available on the CAN-ACx-PCI. They are designed  */
/* for use with the CAN-ACx-PCI and piggy-back boards. These functions are   */
/* not in the standard users manual. For description read the piggy-back     */
/* manuals                                                                   */
__s32 CANL2_init_signals(CAN_HANDLE Can, __u32 ulChannelDirectionMask,
                         __u32 ulChannelOutputDefaults);

__s32 CANL2_read_signals(CAN_HANDLE Can, __u32 *pulChannelRead);

__s32 CANL2_write_signals(CAN_HANDLE Can, __u32 pulChannelWrite,
                          __u32 ulCareMask);

} // extern "C"

#endif // __CANL2_H__
