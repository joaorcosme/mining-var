#ifndef __CAN_UTILS_CHANNEL_H_
#define __CAN_UTILS_CHANNEL_H_

#include "CANL2.h"

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
