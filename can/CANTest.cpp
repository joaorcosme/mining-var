/*
 *   Displays the data transmitted by the BS-9000 sensor in the CAN bus
 *   (for testing purposes).
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

#include "BSFrameHandler.h"
#include "CANUtils.h"
#include "CANproChannel.h"
#include "DetectionGUI.h"

#include <functional> // std::ref
#include <future>
#include <stdexcept>
#include <thread>
#include <utility> // std::move

int main(int argc, char** argv)
{
    static constexpr unsigned N_SENSORS = 1;

    try {
        can::CANproChannel channel;
        can::backsense::RadarStateDB stateDB(N_SENSORS);

        std::promise<void> exitSignal;
        std::future<void> futureSignal = exitSignal.get_future();
        std::thread readingHandler(can::CANUtils::readMsgs,
                                   channel.getHandle(), std::ref(stateDB),
                                   std::move(futureSignal));

        gui::DetectionGUI interface(stateDB);
        // blocking call
        interface.launchGUI();

        // notify interruption thread
        exitSignal.set_value();

        can::CANUtils::resetChip(channel.getHandle());
        readingHandler.join();

    } catch (std::runtime_error& ex) {
        std::cerr << "#ERROR: " << ex.what() << std::endl;
    }

    return 0;
}
