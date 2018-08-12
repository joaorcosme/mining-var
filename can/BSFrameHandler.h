#ifndef _BACKSENSE_FRAME_HANDLER_H_
#define _BACKSENSE_FRAME_HANDLER_H_

#include "BSDataConverter.h"
#include "CANL2.h" // PARAM_STRUCT

#include <linux/types.h>

#include <algorithm>
#include <array>
#include <experimental/optional>
#include <iostream>
#include <set>
#include <unordered_set>

namespace can {

namespace backsense {

static constexpr unsigned MAX_N_OBJS = 8;
static constexpr unsigned MAX_N_SENSORS = 8;

class FrameHandler;

class DetectionData {
  public:
    DetectionData(const DetectionData &other) = default;
    DetectionData &operator=(const DetectionData &) = default;

    bool operator<(const DetectionData &other) const {
        return m_detectionId < other.getId();
    }

    __u32 getId() const { return m_detectionId; }

    double getPolarRadius() const;
    int getPolarAngle() const;
    double getX() const;
    double getY() const;
    double getRelativeSpeed() const;
    int getSignalPower() const;
    int getObjectId() const;
    int getObjectAppearanceStatus() const;
    int getTriggerEvent() const;
    int getDetectionFlag() const;

    void dump(std::ostream& out) const;

  private:
    explicit DetectionData(const __u8 *data, const __u32 detectionId)
        : m_detectionId(detectionId) {
        std::copy(data, data + N_BYTES, m_frame.begin());
    }

    // only the frame handler should build frames directly
    friend FrameHandler;

  private:
    mutable std::array<__u8, N_BYTES> m_frame;
    __u32 m_detectionId;
};

class FrameHandler {
  public:
    FrameHandler(const FrameHandler &) = delete;
    FrameHandler &operator=(const FrameHandler &) = delete;

    FrameHandler() { initializeDetectionIds(); }

    std::experimental::optional<DetectionData>
    processRcvFrame(const PARAM_STRUCT &frame);

  private:
    bool isDetectionObjectId(const __u32 id) const;

    void initializeDetectionIds();

  private:
    std::unordered_set<__u32> m_detectionIds;
};

class RadarStateDB {
  public:
    RadarStateDB() = default;

    void updateState(const DetectionData &&newState);
    auto size() const { return m_db.size(); }

  private:
    void autoClear();

  private:
    std::set<DetectionData> m_db;
    unsigned m_callCount = 0;
};

} // namespace backsense

} // namespace can

#endif // _BACKSENSE_FRAME_HANDLER_H_
