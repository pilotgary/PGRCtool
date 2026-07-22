/**
 * @file display.h
 * @brief OLED display interface.
 */

#pragma once

#include <stdint.h>

#include "types.h"

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
};

bool init();

void splash();

void setData(const Data& data);

void refresh();

} // namespace Display