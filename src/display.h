/**
 * @file display.h
 * @brief OLED display interface.
 */

#pragma once

#include <stdint.h>

#include "app.h"
#include "generator.h"

namespace Display
{

struct Data
{
    App::Mode mode;

    bool signalPresent;
    bool stable;

    uint16_t inputPulseUs;
    uint16_t stablePulseUs;
    uint8_t acquiringSamples;

    uint16_t outputPulseUs;
    Generator::GeneratorMode generatorMode;

    uint32_t framePeriodUs;
    uint16_t frameRateHz;

    uint16_t jitterUs;

    uint16_t minimumPulseUs;
    uint16_t maximumPulseUs;
    uint16_t peakToPeakUs;
    uint32_t totalSamples;
};

bool init();

void splash();

void setData(const Data& data);

void refresh();

} // namespace Display