/**
 * @file generator.h
 * @brief Servo PWM output generator.
 */

#pragma once

#include <stdint.h>

namespace Generator
{

enum class GeneratorMode : uint8_t
{
    Manual,
    Sine,
    Triangular,
    Square
};

bool init();

void update();

void nextPreset();

void nextSweepMode();

void toggleMode();

uint16_t outputPulseUs();

GeneratorMode mode();

} // namespace Generator