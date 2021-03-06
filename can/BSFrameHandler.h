/*
 *   A data structure to represent frames read from the CAN bus and
 *   its handler class.
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
#include <mutex>
#include <unordered_map>
#include <vector>

namespace can {

namespace backsense {

static constexpr unsigned MAX_N_OBJS = 8;
//static constexpr unsigned MAX_N_SENSORS = 8;
// hacking to 1 since we aint using more than 1
static constexpr unsigned MAX_N_SENSORS = 1;

class FrameHandler;

class DetectionData
{
  public:
    DetectionData(const DetectionData& other) = default;
    DetectionData& operator=(const DetectionData&) = default;

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

    void dump(std::ostream& out) const;

  private:
    explicit DetectionData(const __u8* data, const __u32 detectionId)
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
    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;

    FrameHandler() { initializeDetectionIds(); }

    OptDetectionData processRcvFrame(const PARAM_STRUCT& frame);

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

    void updateState(const DetectionData&& newState);

    const DetectionDataVec& getSensorData(unsigned sensorIdx) const;

  private:
    void autoClear();

  public:
    mutable std::mutex mutex_db;

  private:
    std::vector<DetectionDataVec> m_db;
    unsigned m_callCount = 0;
};

} // namespace backsense

} // namespace can

#endif // _BACKSENSE_FRAME_HANDLER_H_
