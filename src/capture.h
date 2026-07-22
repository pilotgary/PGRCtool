/**
 * @file capture.h
 * @brief PWM input capture interface.
 */

#pragma once

#include <stdint.h>

namespace Capture
{

struct Measurement
{
    bool signalPresent;

    bool stable;

    uint16_t pulseWidthUs;

    uint16_t stablePulseWidthUs;

    uint8_t acquiringSamples;

    uint32_t pulseCount;
};

bool init();

void update();

Measurement getMeasurement();

} // namespace Capture