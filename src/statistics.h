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
    // Current
    uint16_t pulseWidthUs;

    // Rolling statistics
    uint16_t averagePulseUs;
    uint16_t minimumPulseUs;
    uint16_t maximumPulseUs;
    uint16_t jitterUs;

    // Frame
    uint32_t framePeriodUs;
    uint16_t frameRateHz;

    // Quality
    bool signalPresent;
    bool stable;

    uint8_t sampleCount;
};

void init();

void update(const Capture::Measurement& measurement);

Data data();

} // namespace Statistics
