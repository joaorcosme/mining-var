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
#include <stdexcept>
#include <iomanip>
#include <thread>
#include <utility>
#include <vector>

int main(int argc, char** argv)
{
    try {
        static constexpr unsigned N_SENSORS = 1;

        can::CANproChannel channel;
        can::backsense::RadarStateDB stateDB(N_SENSORS);

        // Start a task to handle the CAN bus and DB updates
        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread canHandler(can::CANUtils::interruption, channel.getHandle(),
                               std::ref(stateDB), std::move(futureSignal));

        // TODO: extract this section (probably into a separate thread)
        // For now, it's just an experiment to connect the CAN DB and the AR
        // display
        // --------------------------(start)
        cv::Mat frame;
        cv::VideoCapture cap;

        // access built-in camera at index 0
        assert(cap.open(0));

        using namespace std::chrono_literals;
        augreality::SensorSimulator s1(2000ms);
        augreality::BarGraph bGraph(cv::Point(50, 70), 60, 300);
        while (true) {
            cap >> frame;
            assert(!frame.empty());

            auto data = stateDB.getSensorData(0)[0];
            auto frac = 0.0;
            if (data) {
                auto polarRadius = data->getPolarRadius();
                std::stringstream ss;
                ss << std::setprecision(3) << polarRadius << "m";
                bGraph.drawTxt(frame, ss.str());
                frac = polarRadius / 2.0;
            }
            //auto frac = s1.getFraction();
            bGraph.draw(frame, frac);

            cv::imshow("Live", frame);
            if (cv::waitKey(5) == 27) { // Esc
                break;
            }
        }
        // --------------------------(end)

        // notify interruption thread
        exitSignal.set_value();

        can::CANUtils::resetChip(channel.getHandle());
        canHandler.join();

    } catch (std::runtime_error& ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
