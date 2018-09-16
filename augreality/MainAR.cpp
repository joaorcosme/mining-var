/*
 *   An Augmented Reality application to aid mining vehicle operators.
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

#include "BarGraph.h"
#include "SensorSimulator.h"

#include "../can/BSFrameHandler.h"
#include "../can/CANUtils.h"
#include "../can/CANproChannel.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <chrono>
#include <future>
#include <iomanip>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

template <typename T> static std::string buildDisplayTextValue(const T value)
{
    std::stringstream builder;
    builder << std::setprecision(3) << value << "m";
    return builder.str();
}

static void launchARWindowLoop(const can::backsense::RadarStateDB& stateDB)
{
    cv::Mat frame;
    cv::VideoCapture cap;
    // access built-in camera at index 0
    if (!cap.open(0)) {
        throw std::runtime_error("Can't open camera.");
    }

    augreality::BarGraph bGraph(cv::Point(50, 70), 60, 300);

    while (cv::waitKey(5) != 27) { // Esc key
        cap >> frame;
        assert(!frame.empty());
        // take the closest object's data, for the sensor at index 0
        auto detectionData = stateDB.getSensorData(0)[0];
        auto frac = 0.0;
        if (detectionData) {
            auto polarRadius = detectionData->getPolarRadius();
            bGraph.drawTxt(frame, buildDisplayTextValue(polarRadius));

            static constexpr double MAX_RADIUS = 5.0;
            frac = polarRadius / MAX_RADIUS;
        }
        bGraph.draw(frame, frac);
        cv::imshow("Live", frame);
    }
}

int main(int argc, char** argv)
{
    try {
        static constexpr unsigned N_SENSORS = 1;

        can::CANproChannel channel;
        can::backsense::RadarStateDB stateDB(N_SENSORS);

        // start a task to handle the CAN bus and DB updates
        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread canHandler(can::CANUtils::interruption, channel.getHandle(),
                               std::ref(stateDB), std::move(futureSignal));

        // blocking call: loop until the user quits
        launchARWindowLoop(stateDB);

        // notify interruption thread
        exitSignal.set_value();

        can::CANUtils::resetChip(channel.getHandle());
        canHandler.join();

    } catch (std::runtime_error& ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
