#include "BSFrameHandler.h"

#include <cassert>

// :::: class FrameHandler

using can::backsense::DetectionData;
using can::backsense::FrameHandler;

std::experimental::optional<DetectionData>
FrameHandler::processRcvFrame(const PARAM_STRUCT &frame) {
    assert(frame.DataLength == N_BYTES);

    std::experimental::optional<DetectionData> optState;

    if (isDetectionObjectId(frame.Ident)) {
        optState = DetectionData(frame.RCV_data, frame.Ident);
    }
    return optState;
}

bool FrameHandler::isDetectionObjectId(const __u32 id) const {
    return m_detectionIds.find(id) != m_detectionIds.end();
}

void FrameHandler::initializeDetectionIds() {
    __u32 baseId = 0x310;
    for (auto i = 0; i < MAX_N_SENSORS; ++i) {
        __u32 objId = baseId;
        for (auto j = 0; j < MAX_N_OBJS; ++j) {
            m_detectionIds.insert(objId++);
        }
        baseId += 0x10;
    }
}

// :::: class RadarStateDB

using can::backsense::RadarStateDB;

void RadarStateDB::updateState(const DetectionData &&newState) {
    autoClear();

    auto insertionRet = m_db.insert(newState);
    bool inserted = insertionRet.second;

    if (!inserted) {
        auto hint = insertionRet.first;
        ++hint;
        m_db.erase(insertionRet.first);
        m_db.insert(hint, newState);
    }
}

void RadarStateDB::autoClear() {
    // best effort to keep the DB state up-to-date
    if (m_callCount >= MAX_N_OBJS * MAX_N_SENSORS) {
        // TODO: source of glitches (relevant?)
        m_db.clear();
        m_callCount = 0;
    }
    m_callCount++;
}
