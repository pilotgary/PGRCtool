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
    uint16_t pulseWidthUs;
    uint32_t framePeriodUs;
    uint32_t sampleNumber;
};

bool init();

void update();

Measurement getMeasurement();

} // namespace Capture