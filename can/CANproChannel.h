/*
 *   A class to handle interaction with the CAN Layer 2 API.
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

#ifndef _CAN_PRO_CHANNEL_H_
#define _CAN_PRO_CHANNEL_H_

#include "CANL2.h"

namespace can {

class CANproChannel
{
  public:
    CANproChannel& operator=(const CANproChannel) = delete;
    CANproChannel(const CANproChannel&) = delete;

    CANproChannel();
    ~CANproChannel();

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

} // namespace can

#endif // _CAN_PRO_CHANNEL_H_
