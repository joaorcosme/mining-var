#ifndef _BACKSENSE_FRAME_HANDLER_H_
#define _BACKSENSE_FRAME_HANDLER_H_

#include "BSDataConverter.h"
#include "CANL2.h" // PARAM_STRUCT

#include <cassert>
#include <linux/types.h>

#include <algorithm>
#include <array>
#include <experimental/optional>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace can {

namespace backsense {

static constexpr unsigned MAX_N_OBJS = 8;
static constexpr unsigned MAX_N_SENSORS = 8;

class FrameHandler;

class DetectionData
{
  public:
    DetectionData(const DetectionData &other) = default;
    DetectionData &operator=(const DetectionData &) = default;

    __u32 getId() const { return m_detectionId; }
    std::string getStrHexId() const;

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

    void dump(std::ostream &out) const;

  private:
    explicit DetectionData(const __u8 *data, const __u32 detectionId)
        : m_detectionId(detectionId)
    {
        std::copy(data, data + N_BYTES, m_frame.begin());
    }

    // only the frame handler should build frames directly
    friend FrameHandler;

  private:
    std::array<__u8, N_BYTES> m_frame;
    __u32 m_detectionId;
};

using OptDetectionData = std::experimental::optional<DetectionData>;

class FrameHandler
{
  public:
    FrameHandler(const FrameHandler &) = delete;
    FrameHandler &operator=(const FrameHandler &) = delete;

    FrameHandler() { initializeDetectionIds(); }

    OptDetectionData processRcvFrame(const PARAM_STRUCT &frame);

    static std::pair<unsigned, unsigned> getIndexPairFromId(const __u32 id);

  private:
    bool isDetectionObjectId(const __u32 id) const;

    void initializeDetectionIds();

  private:
    // key: frame id --> value: pair of <sensor idx, obj idx>
    static std::unordered_map<__u32, std::pair<unsigned, unsigned>>
        s_detectionIdsToIndexes;
};

using DetectionDataVec = std::vector<OptDetectionData>;
using std::experimental::nullopt;

class RadarStateDB
{
  public:
    RadarStateDB(unsigned nSensors)
    {
        assert(nSensors <= MAX_N_SENSORS);
        m_db.assign(nSensors, DetectionDataVec(MAX_N_OBJS, nullopt));
    }

    void updateState(const DetectionData &&newState);

    const DetectionDataVec &getSensorData(unsigned sensorIdx) const;

  private:
    void autoClear();

  private:
    std::vector<DetectionDataVec> m_db;
    unsigned m_callCount = 0;
};

} // namespace backsense

} // namespace can

#endif // _BACKSENSE_FRAME_HANDLER_H_
