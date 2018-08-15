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

#include "SensorSimulator.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <chrono>
#include <string>
#include <utility>
#include <vector>

class BarGraph
{
  public:
    BarGraph() = delete;
    BarGraph(const cv::Point& startPt, const double width, const double height,
             unsigned nTiles = 25)
    {
        const double totalSpacing = 0.2 * height;
        const double spacing = totalSpacing / (nTiles - 1);
        const double tileHeight = (height - totalSpacing) / nTiles;
        const cv::Size tileSize(width, tileHeight);
        cv::Point pt = startPt;

        m_txtOrg.x = startPt.x;
        m_txtOrg.y = startPt.y - 5;

        unsigned i = 0;
        while (i++ < nTiles) {
            m_tiles.emplace_back(pt, tileSize);
            pt.y += spacing + tileHeight;
        }
    }

    unsigned getNumberOfTiles() { return m_tiles.size(); }

    void draw(cv::Mat& frame, const double fraction);

  private:
    void drawPercentageTxt(cv::Mat& frame, const double fraction);

  private:
    std::vector<cv::Rect> m_tiles;
    cv::Point m_txtOrg;
    static constexpr int m_fontFace = cv::FONT_HERSHEY_SIMPLEX;
};

void BarGraph::draw(cv::Mat& frame, const double fraction)
{
    assert(fraction >= 0 && fraction <= 1);
    const int nFilledTiles = fraction * getNumberOfTiles();
    int i = 0;
    for (const auto& tile : m_tiles) {
        int thickness = 1;
        cv::Scalar tileColor(255, 255, 255);
        if (i++ < nFilledTiles) {
            // negative thickness yields a filled rectangle
            thickness = -1;
            tileColor = cv::Scalar(0, 255, 0);
        }
        cv::rectangle(frame, tile, tileColor, thickness);
    }
    drawPercentageTxt(frame, fraction);
}

void BarGraph::drawPercentageTxt(cv::Mat& frame, const double fraction)
{
    std::string txt = std::to_string(static_cast<int>(100 * fraction));
    txt += "%";
    cv::putText(frame, txt, m_txtOrg, m_fontFace, 1, cv::Scalar(0, 0, 255), 2);
}

int main(int argc, char** argv)
{
    cv::Mat frame;
    cv::VideoCapture cap;

    // access built-in camera at index 0
    assert(cap.open(0));

    BarGraph bGraph1(cv::Point(50, 50), 80, 300);
    BarGraph bGraph2(cv::Point(200, 50), 80, 300);
    BarGraph bGraph3(cv::Point(350, 50), 80, 300);

    using namespace std::chrono_literals;
    augreality::SensorSimulator sensor1(1500ms);
    augreality::SensorSimulator sensor2(1500ms);
    augreality::SensorSimulator sensor3(1500ms);

    while (true) {
        cap >> frame;
        assert(!frame.empty());

        bGraph1.draw(frame, sensor1.getFraction());
        bGraph2.draw(frame, sensor2.getFraction());
        bGraph3.draw(frame, sensor3.getFraction());

        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0) {
            break;
        }
    }

    return 0;
}
