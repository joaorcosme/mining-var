#ifndef __CAN_PRO_CHANNEL_H_
#define __CAN_PRO_CHANNEL_H_

#include "canL2.h"

#include <memory>

class CANproChannel {
  public:
    CANproChannel &operator=(const CANproChannel) = delete;
    CANproChannel(const CANproChannel &) = delete;

    CANproChannel();

    void initializeChannel();
    void printChannelInfo() const;
    CAN_HANDLE getHandle() const { return m_handle; }

  private:
    L2CONFIG getLayer2Configuration() const;
    void setFifoMode();

  private:
    std::unique_ptr<CHDSNAPSHOT> m_pChannel{new CHDSNAPSHOT};
    CAN_HANDLE m_handle;
};

#endif // __CAN_PRO_CHANNEL_H_
