#ifndef _BACKSENSE_CAN_FRAME_H_
#define _BACKSENSE_CAN_FRAME_H_

#include "CANL2.h"

#include <cassert>

#include <algorithm>
#include <array>

namespace can {

static constexpr unsigned NBYTES = 8;

template <typename PhyT> class DetectionData {
  public:
    PhyT convert(const std::array<__u8, NBYTES> &frame) {
        __u8 byte = frame[byteNumber()];

        assert(byte >= minRawValue() && byte <= maxRawValue());

        if (dataLength() != 8) {
            byte >>= startBit();
            byte <<= (8 - dataLength() - startBit());
        }

        PhyT value = byte * resolution() + offset();
    }

  protected:
    virtual PhyT resolution() const = 0;
    virtual __u8 minRawValue() const = 0;
    virtual __u8 maxRawValue() const = 0;
    virtual unsigned byteNumber() const = 0;

    virtual int offset() const { return 0; }
    virtual unsigned dataLength() const { return 8; }
    virtual unsigned startBit() const { return 0; }
};

class PolarRadius : public DetectionData<double> {
  public:
    PolarRadius() = default;

  private:
    double resolution() const override { return 0.25; }
    __u8 minRawValue() const override { return 0x0; }
    __u8 maxRawValue() const override { return 0x79; }
    unsigned byteNumber() const override { return 0; }
};


class BacksenseCANFrame {
  public:
    explicit BacksenseCANFrame(PARAM_STRUCT &data) {
        assert(data.DataLength == NBYTES);
        std::copy(data.RCV_data, data.RCV_data+NBYTES, m_frame.begin());
    }

    double getPolarRadius() const { return PolarRadius().convert(m_frame); }

  private:
    std::array<__u8, NBYTES> m_frame;
};

} // namespace can

#endif // _BACKSENSE_CAN_FRAME_H_
