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

#include "../can/BSFrameHandler.h"
#include "../can/CANUtils.h"
#include "../can/CANproChannel.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <future>
#include <iomanip>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

static void launchARWindowLoop(const can::backsense::RadarStateDB& stateDB)
{
    cv::Mat frame;
    cv::VideoCapture cap;
    // access built-in camera at index 0
    if (!cap.open(0)) {
        throw std::runtime_error("Can't open camera.");
    }

    cap >> frame;

    //
    // 30m ^  +--------------+
    //     |  |              |
    //     |  |              |
    //     |  |              |
    //     |  |              |     X ^
    //     |  |              |       |
    //     |  |              |       |
    //     |  |      O       |       |
    //     |  |      |       |       +------>
    //     |  | O    |       |              Y
    //     |  |  \   |   O   |
    //     |  |   \  |  /    |
    //     |  |    \ | /     |
    //     |  |     \|/      |
    //  0m v  +------S-------+
    //
    //        <-------------->
    //      -5m             +5m
    //

    constexpr unsigned MAX_PHY_X = 10;
    constexpr unsigned MAX_PHY_Y = +5;
    constexpr unsigned MIN_PHY_Y = -5;
    constexpr unsigned DISPLAY_SPACING_X = 20;

    const unsigned sensorX = frame.rows - (DISPLAY_SPACING_X / 2);
    const unsigned sensorY = frame.cols / 2;
    const cv::Point sensorP(sensorY, sensorX);

    const unsigned rectHeight = frame.rows - DISPLAY_SPACING_X;
    const unsigned rectWidth = rectHeight;
    const unsigned pxStepX = rectHeight / MAX_PHY_X;
    const unsigned pxStepY = rectWidth / (MAX_PHY_Y - MIN_PHY_Y);

    const unsigned rectX = DISPLAY_SPACING_X / 2;
    const unsigned rectY = sensorY + (MIN_PHY_Y * pxStepY);
    const cv::Point rectP1(rectY, rectX);
    const cv::Point rectP2(rectY + rectWidth, rectX + rectHeight);
    const cv::Scalar rectColor(0, 0, 255);

    constexpr unsigned obstRadius = 10;
    const cv::Scalar obstColor(0, 255, 255);

    auto toDisplayCoords = [&](const auto& y, const auto& x) {
        const unsigned dispY = (y * pxStepY) + sensorY;
        const unsigned dispX = sensorX - (x * pxStepX);
        return cv::Point(dispY, dispX);
    };

    while (cv::waitKey(5) != 27) { // Esc key
        cap >> frame;
        assert(!frame.empty());
        // get data from 1 sensor only, at index 0
        for (const auto obstacle : stateDB.getSensorData(0)) {
            if (obstacle) {
                auto y = obstacle->getY();
                auto x = obstacle->getX();
                auto obstP = toDisplayCoords(y, x);

                // draw obstacle
                cv::circle(frame, obstP, obstRadius, obstColor,
                           -1 /* filled circle */, cv::LINE_AA /* line type */);
                cv::line(frame, sensorP, obstP, obstColor, 1 /* thickness */,
                         cv::LINE_8 /* line type */);
            }
        }
        cv::rectangle(frame, rectP1, rectP2, rectColor);
        cv::imshow("Augmented Reality App", frame);
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
