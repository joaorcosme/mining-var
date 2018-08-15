/*
 *   Helper functions to handle CAN bus operations.
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

#ifndef __CAN_UTILS_CHANNEL_H_
#define __CAN_UTILS_CHANNEL_H_

#include "CANL2.h"
#include "CANproChannel.h"

#include <array>
#include <string>

namespace can {

class CANUtils
{
  public:
    CANUtils& operator=(const CANUtils) = delete;
    CANUtils(const CANUtils&) = delete;

    CANUtils() = default;

    static int readBusEvent(CAN_HANDLE can, PARAM_STRUCT& retParam);
    static void resetChip(CAN_HANDLE can) { CANL2_reset_chip(can); }
    static void printReceivedData(int frc, const PARAM_STRUCT& param);

  private:
    static std::string formatHexStr(const __u8* data, const __s32 len);

  private:
    static constexpr std::array<char, 16> m_hexMap{'0', '1', '2', '3', '4', '5',
                                                   '6', '7', '8', '9', 'a', 'b',
                                                   'c', 'd', 'e', 'f'};
};

} // namespace can

#endif // __CAN_UTILS_CHANNEL_H_
