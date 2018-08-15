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

#ifndef _DETECTION_GUI_H_
#define _DETECTION_GUI_H_

#include "DetectionGUI.h"

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>

#include <experimental/optional>
#include <vector>

namespace can {

namespace backsense {

class DetectionData;
class RadarStateDB;

} // namespace backsense

} // namespace can

namespace gui {

class DetectionGUI
{
  public:
    DetectionGUI(const DetectionGUI& other) = delete;
    DetectionGUI& operator=(const DetectionGUI&) = delete;

    DetectionGUI(const can::backsense::RadarStateDB& stateDB);
    void launchGUI();

  private:
    // translate data from our DB into text that can be
    // displayed in the "listbox" cells
    static std::vector<nana::listbox::cell> cellTranslator(
        const std::experimental::optional<can::backsense::DetectionData>& data);

  private:
    // TODO: there are probably better ways to define the sizes
    nana::form m_form{nana::rectangle{100, 100, 800, 400}};
    nana::button m_button{m_form, nana::rectangle{370, 350, 60, 30}};
    nana::listbox m_lsbox{m_form, nana::rectangle{25, 40, 750, 250}};
};

} // namespace gui

#endif // _DETECTION_GUI_H_
