/**
 * @file statistics.h
 * @brief PWM signal statistics.
 */

#pragma once

#include <stdint.h>

#include "capture.h"

namespace Statistics
{

struct Data
{
    bool signalPresent;

    bool stable;

    uint16_t pulseWidthUs;

    uint16_t stablePulseUs;

    uint16_t minimumPulseUs;

    uint16_t maximumPulseUs;

    uint16_t jitterUs;

    uint8_t sampleCount;
};

void init();

void update(const Capture::Measurement& measurement);

Data data();

} // namespace Statistics
