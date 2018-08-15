/*
 *   A graphical interface to display physical data from the BS-9000 sensor.
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

#include "DetectionGUI.h"
#include "BSFrameHandler.h"

#include <chrono>
#include <thread>

static void adjustColumns(nana::listbox& lsbox)
{
    for (unsigned i = 0; i < lsbox.column_size(); ++i) {
        lsbox.column_at(i).text_align(::nana::align::center);
        lsbox.column_at(i).fit_content();
    }
}

using gui::DetectionGUI;

DetectionGUI::DetectionGUI(const can::backsense::RadarStateDB& stateDB)
{
    m_button.caption("Quit");
    m_button.events().click([this] { m_form.close(); });

    m_lsbox.append_header("Id");
    m_lsbox.append_header("Radius (m)");
    m_lsbox.append_header("Angle (deg)");
    m_lsbox.append_header("X Coord (m)");
    m_lsbox.append_header("Y Coord (m)");
    m_lsbox.append_header("Relative Speed (km/h)");
    m_lsbox.append_header("Signal Power (db)");
    m_lsbox.append_header("Object Id");
    m_lsbox.append_header("Appearance Status");
    m_lsbox.append_header("Trigger Event");
    m_lsbox.append_header("Detection Flag");

    adjustColumns(m_lsbox);

    m_lsbox.at(0).shared_model<std::recursive_mutex>(stateDB.getSensorData(0),
                                                     cellTranslator);
    nana::API::window_caption(m_form, "Detection Table");
    nana::API::bgcolor(m_form, nana::colors::light_green);

    m_form.show();
}

void DetectionGUI::launchGUI()
{
    std::thread updater([this]() {
        using namespace std::chrono_literals;
        while (true) {
            std::this_thread::sleep_for(250ms);
            nana::API::refresh_window(m_lsbox);
        }
    });

    updater.detach();
    nana::exec();
}

std::vector<nana::listbox::cell> DetectionGUI::cellTranslator(
    const std::experimental::optional<can::backsense::DetectionData>& data)
{
    static constexpr unsigned numParams = 11;
    std::vector<nana::listbox::cell> cells;

    if (data) {
        cells.emplace_back(data->getStrHexId());
        cells.emplace_back(std::to_string(data->getPolarRadius()));
        cells.emplace_back(std::to_string(data->getPolarAngle()));
        cells.emplace_back(std::to_string(data->getX()));
        cells.emplace_back(std::to_string(data->getY()));
        cells.emplace_back(std::to_string(data->getRelativeSpeed()));
        cells.emplace_back(std::to_string(data->getSignalPower()));
        cells.emplace_back(std::to_string(data->getObjectId()));
        cells.emplace_back(std::to_string(data->getObjectAppearanceStatus()));
        cells.emplace_back(std::to_string(data->getTriggerEvent()));
        cells.emplace_back(std::to_string(data->getDetectionFlag()));
    } else {
        for (int i = 0; i < numParams; ++i) {
            cells.emplace_back("");
        }
    }
    return cells;
}
