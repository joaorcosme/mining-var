#include "CANproChannel.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

static std::string getDriverErrorMsg(const int code) {
    return "The driver reported a problem. Error Code: " + std::to_string(code);
}

CANproChannel::CANproChannel() {
    try {

        queryChannel();
        printChannelInfo();
        initializeChannel();
        setFifoMode();

    } catch (...) {
        throw;
    }
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

    const __u32 providedBufferSize = neededBufferSize;

    // now call the function with a valid buffer size and pointer to channel
    retCode = CANL2_get_all_CAN_channels(providedBufferSize, &neededBufferSize,
                                         &nChannels, m_pChannel.get());
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
    std::cout << "#INFO: Initializing channel " << name << std::endl;

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

void CANproChannel::setFifoMode() const {
    L2CONFIG config = getLayer2Configuration();
    int retCode = CANL2_initialize_fifo_mode(m_handle, &config);
    if (retCode) {
        throw std::runtime_error("#ERROR " + std::to_string(retCode) +
                                 " in INIL2_initialize_fifo_mode()");
    }
}

L2CONFIG CANproChannel::getLayer2Configuration() const {
    L2CONFIG l2Config;
    l2Config.bEnableAck = GET_FROM_SCIM;
    l2Config.bEnableErrorframe = GET_FROM_SCIM;
    l2Config.s32AccCodeStd = GET_FROM_SCIM;
    l2Config.s32AccCodeXtd = GET_FROM_SCIM;
    l2Config.s32AccMaskStd = GET_FROM_SCIM;
    l2Config.s32AccMaskXtd = GET_FROM_SCIM;
    l2Config.s32OutputCtrl = GET_FROM_SCIM;
    l2Config.s32Prescaler = GET_FROM_SCIM;
    l2Config.s32Sam = GET_FROM_SCIM;
    l2Config.s32Sjw = GET_FROM_SCIM;
    l2Config.s32Tseg1 = GET_FROM_SCIM;
    l2Config.s32Tseg2 = GET_FROM_SCIM;
/*
    L2CONFIG l2Config;
    l2Config.fBaudrate = 500.0;
    l2Config.s32Prescaler = 8;
    l2Config.s32Tseg1 = 11;
    l2Config.s32Tseg2 = 8;
    l2Config.s32Sjw = 2;
    l2Config.s32Sam = 0;
    l2Config.s32AccCodeStd = 0;
    l2Config.s32AccCodeXtd = 0;
    l2Config.s32AccMaskStd = 0;
    l2Config.s32AccMaskXtd = 0;
    l2Config.s32OutputCtrl = 0xFA;
    l2Config.bEnableAck = 1;
    l2Config.bEnableErrorframe = 0;
    */

    return l2Config;
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
