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

#include "CANproChannel.h"
#include "CANUtils.h"

#include <cassert>
#include <iomanip>
#include <iostream>

using can::CANUtils;

constexpr std::array<char, 16> CANUtils::m_hexMap;

int CANUtils::readBusEvent(CAN_HANDLE can, PARAM_STRUCT& retParam)
{
    retParam.DataLength = 3;
    return CANL2_read_ac(can, &retParam);
}

std::string CANUtils::formatHexStr(const __u8* data, const __s32 len)
{
    // input: an array of bytes, like: |100|045|099|000|253|022|009|150|
    // each byte is a __u8 type, which corresponds to a char
    //
    // output: a string where each byte is represented in its hexadecimal
    // representation

    constexpr unsigned MAXLEN = 8;
    assert(len <= MAXLEN);

    // each byte will take 3 positions in the string, like:
    // "64 2D 63 00 FD 16 09 96"
    std::string hexData(3 * MAXLEN, ' ');
    for (unsigned i = 0; i < len; ++i) {
        hexData[3 * i] = m_hexMap[(data[i] & 0xF0) >> 4];
        hexData[3 * i + 1] = m_hexMap[data[i] & 0x0F];
    }
    return hexData;
}

void CANUtils::printReceivedData(int frc, const PARAM_STRUCT& param)
{
    // for now, we are only interested in this type of frame
    if (frc == CANL2_RA_DATAFRAME) {
        std::cout << "RCV STD CAN1 :::: "
                  << "ID " << std::hex << std::setfill(' ') << std::setw(5)
                  << param.Ident << " :: LEN " << param.DataLength
                  << " :: DATA "
                  << formatHexStr(param.RCV_data, param.DataLength)
                  << std::endl;
    } else {
        assert(false);
    }
}
