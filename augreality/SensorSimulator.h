/*
 *   A sensor simulator to generate random data values as a separate task.
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

#ifndef _SENSOR_SIMULATOR_H_
#define _SENSOR_SIMULATOR_H_

#include <atomic>
#include <chrono>
#include <thread>

namespace augreality {

class SensorSimulator
{
  public:
    SensorSimulator(const SensorSimulator& other) = delete;
    SensorSimulator& operator=(const SensorSimulator&) = delete;

    SensorSimulator(std::chrono::milliseconds interval);
    ~SensorSimulator();

    double getFraction() { return m_fraction.load(); }

  private:
    void updateFraction();

  private:
    std::thread m_thread;
    std::chrono::milliseconds m_msInterval;
    std::atomic<double> m_fraction;
    std::atomic<bool> m_leaving{false};
};

} // namespace augreality

#endif // _SENSOR_SIMULATOR_H_
