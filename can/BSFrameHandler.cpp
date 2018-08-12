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

// :::: class DetectionData

double DetectionData::getPolarRadius() const {
    return converter::PolarRadius().convert(m_frame);
}

int DetectionData::getPolarAngle() const {
    return converter::PolarAngle().convert(m_frame);
}

double DetectionData::getX() const { return converter::X().convert(m_frame); }

double DetectionData::getY() const { return converter::Y().convert(m_frame); }

double DetectionData::getRelativeSpeed() const {
    return converter::RelativeSpeed().convert(m_frame);
}

int DetectionData::getSignalPower() const {
    return converter::SignalPower().convert(m_frame);
}

int DetectionData::getObjectId() const {
    return converter::ObjectId().convert(m_frame);
}

int DetectionData::getObjectAppearanceStatus() const {
    return converter::ObjectAppearanceStatus().convert(m_frame);
}

int DetectionData::getTriggerEvent() const {
    return converter::TriggerEvent().convert(m_frame);
}

int DetectionData::getDetectionFlag() const {
    return converter::DetectionFlag().convert(m_frame);
}

void DetectionData::dump(std::ostream &out) const {
    auto objid = getObjectId();
    auto appea = getObjectAppearanceStatus();
    auto trigg = getTriggerEvent();
    auto detec = getDetectionFlag();
    out << "Id: " << std::hex << getId() << std::dec
        << "\nPolar Radius: " << getPolarRadius()
        << "\nPolar Angle: " << getPolarAngle() << "\nX: " << getX()
        << "\nY: " << getY() << "\nRelative Speed: " << getRelativeSpeed()
        << "\nSignal Power: " << getSignalPower()
        << "\nObject Id: " << getObjectId()
        << "\nObject Appearance: " << getObjectAppearanceStatus()
        << "\nTrigger Event: " << getTriggerEvent()
        << "\nDetection Flag: " << getDetectionFlag()
        << "\n----------------------------------------" << std::endl;
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
