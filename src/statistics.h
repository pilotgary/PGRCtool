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
    uint16_t pulseWidthUs;

    uint16_t averagePulseUs;
    uint16_t minimumPulseUs;
    uint16_t maximumPulseUs;
    uint16_t peakToPeakUs;
    uint16_t jitterUs;

    uint32_t totalSamples;

    uint32_t framePeriodUs;
    uint16_t frameRateHz;

    bool signalPresent;
    bool stable;

    uint8_t sampleCount;
};

void init();

void reset();

void update(const Capture::Measurement& measurement);

bool readData(Data& data);

Data data();

} // namespace Statistics