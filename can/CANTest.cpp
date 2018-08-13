#include "BSFrameHandler.h"
#include "CANL2.h"
#include "CANUtils.h"
#include "CANproChannel.h"

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>

#include <cerrno>
#include <cstring>
#include <sys/poll.h>

#include <chrono>
#include <experimental/optional>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

#define DEBUG_INTERRUPTION(MSG)                                                \
  if (0)                                                                       \
  std::cout << "#DEBUG: Interruption Thread :::: " << MSG << std::endl

static bool shouldTerminate(const std::future<void> &signal)
{
  return signal.wait_for(std::chrono::system_clock::duration::zero()) ==
         std::future_status::ready;
}

namespace can {

static void interruption(CAN_HANDLE channel, backsense::RadarStateDB &stateDB,
                         std::future<void> futureSignal)
{
  DEBUG_INTERRUPTION("Thread start");

  struct pollfd can_poll;

  backsense::FrameHandler frameHandler;

  while (!shouldTerminate(futureSignal)) {

    can_poll.fd = CANL2_handle_to_descriptor(channel);
    can_poll.events = POLLIN | POLLHUP;

    int ret = 0;

    DEBUG_INTERRUPTION("Poll section");
    // wait for event on file descriptor
    while (ret <= 0) {
      ret = poll(&can_poll, 1 /*nfds*/, -1 /*timeout*/);

      if (can_poll.revents & POLLHUP) {
        goto endthread;
      }
      if ((ret == -1) && (errno != EINTR)) {
        std::cerr << "#Error: poll() [" << std::strerror(errno) << "]";
      }
    }

    DEBUG_INTERRUPTION("Read section");
    // desciptor is ready to be read
    PARAM_STRUCT outParam;
    while ((ret = CANUtils::readBusEvent(channel, outParam))) {
      if (ret < 0 || shouldTerminate(futureSignal)) {
        goto endthread;
      }

      CANUtils::printReceivedData(ret, outParam);

      auto state = frameHandler.processRcvFrame(outParam);
      if (state) {
        std::ostringstream ss;
        state->dump(ss);
        std::cout << ss.str();
        stateDB.updateState(std::move(*state));
      }
    }
  }

endthread:
  DEBUG_INTERRUPTION("Thread end");
  return;
}

} // namespace can

int main(int argc, char **argv)
{

  try {
    can::CANproChannel channel;
    can::backsense::RadarStateDB stateDB(1 /* nSensors*/);
    std::promise<void> exitSignal;
    std::future<void> futureSignal = exitSignal.get_future();
    std::thread interruptionHandler(can::interruption, channel.getHandle(),
                                    std::ref(stateDB), std::move(futureSignal));
    ///// experiment begin
    nana::form fm(nana::rectangle{100, 100, 800, 400});

    nana::label lab{fm, "Label"};
    lab.format(true);

    nana::label label{fm, nana::rectangle(25, 10, 100, 20)};
    label.caption("Detection Table");

    nana::button btn{fm, nana::rectangle{370, 350, 60, 30}};
    btn.caption("Quit");
    btn.events().click([&fm] { fm.close(); });

    auto cell_translator = [](
        const std::experimental::optional<can::backsense::DetectionData>
            &data) {

      constexpr unsigned numParams = 5;
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
    };

    nana::listbox lsbox{fm, nana::rectangle{25, 40, 750, 250}};

    lsbox.append_header("Id");
    lsbox.append_header("Radius");
    lsbox.append_header("Angle");
    lsbox.append_header("X Coord");
    lsbox.append_header("Y Coord");
    lsbox.append_header("Relative Speed");
    lsbox.append_header("Signal Power");
    lsbox.append_header("Object Id");
    lsbox.append_header("Appearance Status");
    lsbox.append_header("Trigger Event");
    lsbox.append_header("Detection Flag");

    lsbox.at(0).shared_model<std::recursive_mutex>(stateDB.getSensorData(0),
                                                   cell_translator);

    fm.show();

    std::thread updater([&lsbox]() {
      using namespace std::chrono_literals;
      while (true) {
        std::this_thread::sleep_for(1s);
        nana::API::refresh_window(lsbox);
      }
    });
    updater.detach();

    nana::exec();
    ///// experiment end

    std::getchar();

    exitSignal.set_value();
    can::CANUtils::resetChip(channel.getHandle());

    interruptionHandler.join();

  } catch (std::runtime_error &ex) {
    std::cerr << "#ERROR: " << ex.what() << std::endl;
  }

  return 0;
}
