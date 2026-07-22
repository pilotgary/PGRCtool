#pragma once

/**
 * @file types.h
 * @brief Shared data structures.
 */

#include <stdint.h>

struct Measurement
{
    uint32_t timestampUs;

    uint16_t pulseWidthUs;

    uint16_t periodUs;

    bool signalPresent;
};

struct Statistics
{
    uint16_t minimumUs;

    uint16_t maximumUs;

    float averageUs;

    float jitterUs;

    uint32_t sampleCount;
};

struct ScopeSample
{
    uint32_t timestampUs;

    uint16_t pulseWidthUs;

    uint16_t periodUs;
};

namespace App
{

enum class Mode : uint8_t
{
    Scope,
    Generator
};

}