#ifndef _BACKSENSE_CAN_FRAME_H_
#define _BACKSENSE_CAN_FRAME_H_

#include "CANL2.h"

#include <cassert>
#include <linux/types.h>

#include <algorithm>
#include <array>
#include <experimental/optional>
#include <set>
#include <unordered_set>

namespace can {

namespace backsense {

static constexpr unsigned NBYTES = 8;

template <typename PhyT> class DetectionDataConverter {
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

class PolarRadius : public DetectionDataConverter<double> {
  public:
    PolarRadius() = default;

  private:
    double resolution() const override { return 0.25; }
    __u8 minRawValue() const override { return 0x0; }
    __u8 maxRawValue() const override { return 0x79; }
    unsigned byteNumber() const override { return 0; }
};

class DetectionData {
  public:
    DetectionData(const DetectionData &other) = default;
    DetectionData &operator=(const DetectionData &) = default;

  public:
    __u32 getId() const { return m_detectionId; }

    bool operator<(const DetectionData &other) const {
        return m_detectionId < other.getId();
    }

    double getPolarRadius() const { return PolarRadius().convert(m_frame); }

    // private:
    explicit DetectionData(const __u8 *data, const __u32 detectionId)
        : m_detectionId(detectionId) {
        std::copy(data, data + NBYTES, m_frame.begin());
    }

    // only the manager will build frames directly
    // friend auto FrameHandler::processRcvData(const PARAM_STRUCT &);

  private:
    mutable std::array<__u8, NBYTES> m_frame;
    __u32 m_detectionId;
};

class FrameHandler {
  public:
    FrameHandler(const FrameHandler &) = delete;
    FrameHandler &operator=(const FrameHandler &) = delete;

    FrameHandler() { initializeDetectionIds(); }

    auto processRcvFrame(const PARAM_STRUCT &frame) {
        assert(frame.DataLength == NBYTES);

        std::experimental::optional<DetectionData> optState;

        if (isDetectionObjectId(frame.Ident)) {
            optState = DetectionData(frame.RCV_data, frame.Ident);
        }
        return optState;
    }

  private:
    bool isDetectionObjectId(const __u32 id) const {
        return m_detectionIds.find(id) != m_detectionIds.end();
    }

    void initializeDetectionIds() {
        __u32 baseId = 0x310;
        for (auto i = 0; i < MAX_N_SENSORS; ++i) {
            __u32 objId = baseId;
            for (auto j = 0; j < MAX_N_OBJS; ++j) {
                m_detectionIds.insert(objId++);
            }
            baseId += 0x10;
        }
    }

  private:
    std::unordered_set<__u32> m_detectionIds;
    const unsigned MAX_N_OBJS = 8;
    const unsigned MAX_N_SENSORS = 8;
};

class RadarStateDB {
  public:
    void updateState(const DetectionData &&newState) {
        auto insertionRet = m_db.insert(newState);
        bool inserted = insertionRet.second;

        if (!inserted) {
            auto hint = insertionRet.first;
            ++hint;
            m_db.erase(insertionRet.first);
            m_db.insert(hint, newState);
        }
    }

    auto size() const { return m_db.size(); }

  private:
    std::set<DetectionData> m_db;
};

} // namespace backsense

} // namespace can

#endif // _BACKSENSE_CAN_FRAME_H_
