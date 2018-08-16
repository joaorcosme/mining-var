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

#include "SensorSimulator.h"

#include <random>

static double generateRandom(double min, double max)
{
    std::random_device rdev;
    std::mt19937 rgen(rdev());
    std::uniform_real_distribution<> iDist(min, max);
    return iDist(rgen);
}

using augreality::SensorSimulator;

SensorSimulator::SensorSimulator(std::chrono::milliseconds interval)
    : m_thread(&SensorSimulator::updateFraction, this)
    , m_msInterval(interval)
{
    m_fraction.store(0.0);
}

SensorSimulator::~SensorSimulator()
{
    m_leaving.store(true);
    m_thread.join();
}

void SensorSimulator::updateFraction()
{
    while (!m_leaving.load()) {
        std::this_thread::sleep_for(m_msInterval);
        m_fraction.store(generateRandom(0.0, 1.0));
    }
}
