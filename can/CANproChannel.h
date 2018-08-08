#ifndef __CAN_PRO_CHANNEL_H_
#define __CAN_PRO_CHANNEL_H_

#include "canL2.h"

class CANproChannel {
  public:
    CANproChannel &operator=(const CANproChannel) = delete;
    CANproChannel(const CANproChannel &) = delete;

    CANproChannel();
    ~CANproChannel() { delete m_pChannel; }

    void printChannelInfo() const;
    CAN_HANDLE getHandle() const { return m_handle; }

  private:
    void initializeChannel();
    void queryChannel();
    void setFifoMode();
    void setLayer2Configuration();

  private:
    CAN_HANDLE m_handle;
    L2CONFIG m_l2Config;
    CHDSNAPSHOT* m_pChannel{new CHDSNAPSHOT};
};

#endif // __CAN_PRO_CHANNEL_H_
