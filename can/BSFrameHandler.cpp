#include "BSFrameHandler.h"

#include <sstream>
#include <utility>

// :::: class FrameHandler

using can::backsense::DetectionData;
using can::backsense::FrameHandler;

std::unordered_map<__u32, std::pair<unsigned, unsigned>>
    FrameHandler::s_detectionIdsToIndexes;

std::experimental::optional<DetectionData>
FrameHandler::processRcvFrame(const PARAM_STRUCT &frame)
{
    assert(frame.DataLength == N_BYTES);

    OptDetectionData optState;

    if (isDetectionObjectId(frame.Ident)) {
        optState = DetectionData(frame.RCV_data, frame.Ident);
    }
    return optState;
}

std::pair<unsigned, unsigned> FrameHandler::getIndexPairFromId(const __u32 id)
{
    auto retPair = s_detectionIdsToIndexes.find(id);
    assert(retPair != s_detectionIdsToIndexes.end());
    return retPair->second;
}

bool FrameHandler::isDetectionObjectId(const __u32 id) const
{
    return s_detectionIdsToIndexes.find(id) != s_detectionIdsToIndexes.end();
}

void FrameHandler::initializeDetectionIds()
{
    __u32 baseId = 0x310;
    for (auto i = 0; i < MAX_N_SENSORS; ++i) {
        __u32 objId = baseId;
        for (auto j = 0; j < MAX_N_OBJS; ++j) {
            s_detectionIdsToIndexes.emplace(objId++, std::make_pair(i, j));
        }
        baseId += 0x10;
    }
}

// :::: class DetectionData

std::string DetectionData::getStrHexId() const
{
    std::stringstream ss;
    ss << std::hex << "0x" << m_detectionId;
    return ss.str();
}

double DetectionData::getPolarRadius() const
{
    return converter::PolarRadius().convert(m_frame);
}

int DetectionData::getPolarAngle() const
{
    return converter::PolarAngle().convert(m_frame);
}

double DetectionData::getX() const { return converter::X().convert(m_frame); }

double DetectionData::getY() const { return converter::Y().convert(m_frame); }

double DetectionData::getRelativeSpeed() const
{
    return converter::RelativeSpeed().convert(m_frame);
}

int DetectionData::getSignalPower() const
{
    return converter::SignalPower().convert(m_frame);
}

int DetectionData::getObjectId() const
{
    return converter::ObjectId().convert(m_frame);
}

int DetectionData::getObjectAppearanceStatus() const
{
    return converter::ObjectAppearanceStatus().convert(m_frame);
}

int DetectionData::getTriggerEvent() const
{
    return converter::TriggerEvent().convert(m_frame);
}

int DetectionData::getDetectionFlag() const
{
    return converter::DetectionFlag().convert(m_frame);
}

void DetectionData::dump(std::ostream &out) const
{
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

void RadarStateDB::updateState(const DetectionData &&newState)
{
    autoClear();

    auto id = newState.getId();
    auto idxPair = FrameHandler::getIndexPairFromId(id);
    m_db[idxPair.first][idxPair.second] = OptDetectionData(newState);
}

const std::vector<std::experimental::optional<DetectionData>> &
RadarStateDB::getSensorData(unsigned sensorIdx) const
{
    assert(sensorIdx < m_db.size());
    return m_db[sensorIdx];
}

void RadarStateDB::autoClear()
{
    // best effort to keep the DB state up-to-date
    if (m_callCount >= MAX_N_OBJS * MAX_N_SENSORS) {
        // TODO: source of glitches (relevant?)
        m_db.assign(m_db.size(), DetectionDataVec(MAX_N_OBJS, nullopt));
        m_callCount = 0;
    } else {
        m_callCount++;
    }
}
