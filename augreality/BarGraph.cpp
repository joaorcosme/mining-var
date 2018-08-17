/*
 *   A helper class to represent and draw a bar graph in a cv::Mat.
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

#include <opencv2/imgproc/imgproc.hpp>

#include <string>

static auto getColorFromFraction(const double fraction)
{
    if (fraction > 2.0 / 3.0) {
        return cv::Scalar(0, 255, 0); // green
    }
    if (fraction > 1.0 / 3.0) {
        return cv::Scalar(0, 255, 255); // yellow
    }
    return cv::Scalar(0, 0, 255); // red
}

using augreality::BarGraph;

BarGraph::BarGraph(const cv::Point& startPt, const double width,
                   const double height, const unsigned nTiles,
                   const bool upsideDown)
{
    const double totalSpacing = 0.3 * height;
    const double spacing = totalSpacing / (nTiles - 1);
    const double tileHeight = (height - totalSpacing) / nTiles;
    const cv::Size tileSize(width, tileHeight);

    m_txtOrg.x = startPt.x - 3;
    m_txtOrg.y = startPt.y - 8;

    cv::Point pt = startPt;
    unsigned i = 0;
    while (i++ < nTiles) {
        if (upsideDown) {
            m_tiles.emplace_back(pt, tileSize);
        } else {
            m_tiles.emplace_front(pt, tileSize);
        }
        pt.y += spacing + tileHeight;
    }
}

void BarGraph::draw(cv::Mat& frame, const double fraction)
{
    assert(fraction >= 0 && fraction <= 1);
    m_fillColor = getColorFromFraction(fraction);
    const int nTilesToBeFilled = fraction * getNumberOfTiles();
    int i = 0;
    for (const auto& tile : m_tiles) {
        // negative thickness yields a filled rectangle
        int thickness = (i++ < nTilesToBeFilled) ? -1 : 1;
        cv::rectangle(frame, tile,
                      (thickness > 0 ? m_borderColor : m_fillColor), thickness);
    }
}

void BarGraph::drawPercentageTxt(cv::Mat& frame, const double fraction)
{
    drawTxt(frame, std::to_string(static_cast<int>(100 * fraction)) + "%");
}

void BarGraph::drawTxt(cv::Mat& frame, const std::string& txt)
{
    cv::putText(frame, txt, m_txtOrg, m_fontFace, 1, cv::Scalar(158, 46, 33), 2);
}
