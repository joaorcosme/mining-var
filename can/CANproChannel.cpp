#include "CANproChannel.h"

#include <cassert>
#include <iostream>
#include <string>

using can::CANproChannel;

static std::string getDriverErrorMsg(const int code) {
    return "The driver reported a problem. Error Code: " + std::to_string(code);
}

CANproChannel::CANproChannel() {
    try {
        queryChannel();
        printChannelInfo();
        initializeChannel();
        setFifoMode();

        std::cout << "#INFO: The CANpro channel is now online." << std::endl;
    } catch (...) {
        throw;
    }
}

CANproChannel::~CANproChannel() {
    std::cout << "#INFO: Closing CANpro channel." << std::endl;
    INIL2_close_channel(m_handle);

    delete m_pChannel;
}

void CANproChannel::queryChannel() {
    __u32 neededBufferSize, nChannels;

    // first, call the function without a valid
    // buffer size to get the required buffer size
    int retCode =
        CANL2_get_all_CAN_channels(0, &neededBufferSize, &nChannels, nullptr);

    std::string errorMsg;
    if (retCode) {
        errorMsg = getDriverErrorMsg(retCode);
    } else if (!nChannels) {
        errorMsg = "No Softing CAN interface card is plugged in.";
    } else if (nChannels > 1) {
        errorMsg = "More than 1 channel found.";
    }

    if (!errorMsg.empty()) {
        throw std::runtime_error(errorMsg);
    }

    assert(nChannels == 1);
    assert(neededBufferSize == sizeof(*m_pChannel));

    const auto providedBufferSize = neededBufferSize;

    // now call the function with a valid buffer size and pointer to channel
    retCode = CANL2_get_all_CAN_channels(providedBufferSize, &neededBufferSize,
                                         &nChannels, m_pChannel);
    if (retCode) {
        errorMsg = getDriverErrorMsg(retCode);
    } else if (m_pChannel->bIsOpen) {
        errorMsg = "CAN channel used by other applications.";
    }

    if (!errorMsg.empty()) {
        throw std::runtime_error(errorMsg);
    }

    assert(m_pChannel->u32DeviceType == CANPROUSB);
    std::cout << "\n#INFO: 1 channel was found." << std::endl;
}

void CANproChannel::initializeChannel() {
    std::string name((char *)m_pChannel->ChannelName);
    std::cout << "#INFO: Initializing channel \"" << name << "\"." << std::endl;

    CANL2_CH_STRUCT channel;
    name.copy((char *)channel.sChannelName, name.length());

    // we might as well use a custom channel name here...

    int retCode = INIL2_initialize_channel(&channel.ulChannelHandle,
                                           (char *)channel.sChannelName);

    if (retCode) {
        throw std::runtime_error("#ERROR " + std::to_string(retCode) +
                                 " in INIL2_initialize_channel()");
    }

    m_handle = channel.ulChannelHandle;
}

void CANproChannel::setFifoMode() {
    setLayer2Configuration();
    int retCode = CANL2_initialize_fifo_mode(m_handle, &m_l2Config);
    if (retCode) {
        throw std::runtime_error("#ERROR " + std::to_string(retCode) +
                                 " in INIL2_initialize_fifo_mode()");
    }
}

void CANproChannel::setLayer2Configuration() {

    m_l2Config.fBaudrate = 500.00;
    m_l2Config.s32Prescaler = 8;
    m_l2Config.s32Tseg1 = 11;
    m_l2Config.s32Tseg2 = 8;
    m_l2Config.s32Sjw = GET_FROM_SCIM; // TODO: figure out why this won't accept
                                       // a custom value. If I set '2' directly,
                                       // the program is not able to read any
                                       // data from the CAN bus.
    m_l2Config.s32Sam = 0;
    m_l2Config.s32AccCodeStd = 0;
    m_l2Config.s32AccCodeXtd = 0;
    m_l2Config.s32AccMaskStd = 0;
    m_l2Config.s32AccMaskXtd = 0;
    m_l2Config.s32OutputCtrl = 0xFA;
    m_l2Config.bEnableAck = 1;
    m_l2Config.bEnableErrorframe = 0;
}

void CANproChannel::printChannelInfo() const {
    std::cout << "---------------------------------------------\n";
    std::cout << "---- Softing CANproSUB Channel Info\n";
    std::cout << "---- Name: " << m_pChannel->ChannelName << "\n";
    std::cout << "---- Serial Number: " << m_pChannel->u32Serial << "\n";
    std::cout << "---- Channel Number: " << m_pChannel->u32PhysCh << "\n";
    std::cout << "---- Open: " << m_pChannel->bIsOpen << "\n";
    std::cout << "---------------------------------------------\n";
}
