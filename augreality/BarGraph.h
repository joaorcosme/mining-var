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

#ifndef _BAR_GRAPH_H_
#define _BAR_GRAPH_H_

#include <opencv2/core.hpp>

#include <vector>

namespace augreality {

class BarGraph
{
  public:
    BarGraph(const cv::Point& startPt, const double width, const double height,
             unsigned nTiles = 25);

    unsigned getNumberOfTiles() const { return m_tiles.size(); }

    void draw(cv::Mat& frame, const double fraction);

  private:
    void drawPercentageTxt(cv::Mat& frame, const double fraction);

  private:
    std::vector<cv::Rect> m_tiles;
    cv::Point m_txtOrg;
    static constexpr int m_fontFace = cv::FONT_HERSHEY_SIMPLEX;
};

} // namespace augreality

#endif // _BAR_GRAPH_H_
