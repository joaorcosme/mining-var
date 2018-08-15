#ifndef _DETECTION_GUI_H_
#define _DETECTION_GUI_H_

#include <experimental/optional>

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>

namespace can {
namespace backsense {
class DetectionData;
class RadarStateDB;
}
}

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
    // TODO: there are probably better ways to define the
    // sizes, but it doesn't really matter right now
    nana::form m_form{nana::rectangle{100, 100, 800, 400}};
    nana::button m_button{m_form, nana::rectangle{370, 350, 60, 30}};
    nana::listbox m_lsbox{m_form, nana::rectangle{25, 40, 750, 250}};
};

} // namespace gui

#endif // _DETECTION_GUI_H_
