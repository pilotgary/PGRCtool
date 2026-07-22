/**
 * @file generator.h
 * @brief Servo PWM output generator.
 */

#pragma once

#include <stdint.h>

namespace Generator
{

enum class Mode : uint8_t
{
    Fixed,
    Sweep,
    Sine
};

bool init();

void update();

void nextPreset();

void nextMode();

uint16_t outputPulseUs();

Mode mode();

} // namespace Generator