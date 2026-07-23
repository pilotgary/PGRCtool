#include "statistics.h"

#include "config.h"

#include <limits>

namespace
{

Statistics::Data statistics;

uint32_t lastSampleNumber = 0;

uint16_t samples[Config::StableSamples];

uint8_t nextIndex = 0;
uint8_t bufferCount = 0;

bool previousSignalPresent = false;

}

namespace Statistics
{

void init()
{
    statistics = {};

    nextIndex = 0;

    bufferCount = 0;

    for (auto &sample : samples)
    {
        sample = 0;
    }
}

void update(const Capture::Measurement& measurement)
{
    statistics.signalPresent = measurement.signalPresent;

    if (measurement.sampleNumber == lastSampleNumber)
    {
        return;
    }    
    
    lastSampleNumber = measurement.sampleNumber;

    statistics.signalPresent = measurement.signalPresent;

    // Update values that are valid on every sample.
    statistics.pulseWidthUs  = measurement.pulseWidthUs;
    statistics.framePeriodUs = measurement.framePeriodUs;

    if (measurement.framePeriodUs != 0)
    {
        statistics.frameRateHz =
            static_cast<uint16_t>(
                1000000UL / measurement.framePeriodUs);
    }
    else
    {
        statistics.frameRateHz = 0;
    }

    // Lost signal - clear the rolling window.
    if (!measurement.signalPresent)
    {
        if (previousSignalPresent)
        {
            // Clear buffer once.
        }

        previousSignalPresent = false;
        return;
    }

    previousSignalPresent = true;

    // Add the latest pulse to the circular buffer.
    samples[nextIndex] = measurement.pulseWidthUs;

    nextIndex++;

    if (nextIndex >= Config::StableSamples)
    {
        nextIndex = 0;
    }

    if (bufferCount < Config::StableSamples)
    {
        bufferCount++;
    }

    statistics.sampleCount = bufferCount;

    // Wait until the buffer is full.
    if (bufferCount < Config::StableSamples)
    {
        statistics.stable = false;
        return;
    }

    // Recalculate statistics from the rolling window.
    uint32_t sum = 0;

    uint16_t minimum = UINT16_MAX;
    uint16_t maximum = 0;

    for (uint8_t i = 0; i < Config::StableSamples; ++i)
    {
        const uint16_t pulse = samples[i];

        sum += pulse;

        if (pulse < minimum)
        {
            minimum = pulse;
        }

        if (pulse > maximum)
        {
            maximum = pulse;
        }
    }

    statistics.averagePulseUs =
        static_cast<uint16_t>(
            sum / Config::StableSamples);

    statistics.minimumPulseUs = minimum;
    statistics.maximumPulseUs = maximum;
    statistics.jitterUs       = maximum - minimum;

    statistics.stable =
        statistics.jitterUs <=
        Config::StableSpreadUs;
}
Data data()
{
    return statistics;
}

}