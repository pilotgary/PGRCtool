#include "statistics.h"

#include "config.h"

#include <limits>

namespace
{

Statistics::Data statistics;

uint32_t lastSampleNumber = 0;

uint32_t sum = 0;

uint16_t minimum =
    std::numeric_limits<uint16_t>::max();

uint16_t maximum = 0;

}

namespace Statistics
{

void init()
{
    statistics = {};

    sum = 0;

    minimum = std::numeric_limits<uint16_t>::max();

    maximum = 0;

}

void update(const Capture::Measurement& measurement)
{
        if (measurement.sampleNumber == lastSampleNumber)
    {
        return;
    }

    lastSampleNumber = measurement.sampleNumber;

    statistics.signalPresent =
        measurement.signalPresent;

    if (!measurement.signalPresent)
    {
        statistics.sampleCount = 0;
        statistics.stablePulseUs = 0;
        statistics.minimumPulseUs = 0;
        statistics.maximumPulseUs = 0;
        statistics.jitterUs = 0;

        statistics.stable = false;

        sum = 0;

        minimum =
            std::numeric_limits<uint16_t>::max();

        maximum = 0;

        return;
    }

    const uint16_t pulse =
        measurement.pulseWidthUs;

    statistics.pulseWidthUs = pulse;

    sum += pulse;

    if (pulse < minimum)
    {
        minimum = pulse;
    }

    if (pulse > maximum)
    {
        maximum = pulse;
    }

    ++statistics.sampleCount;

        if (statistics.sampleCount <
        Config::StableSamples)
    {
        return;
    }

    statistics.stablePulseUs =
        sum / Config::StableSamples;

    statistics.minimumPulseUs =
        minimum;

    statistics.maximumPulseUs =
        maximum;

    statistics.jitterUs =
        maximum - minimum;

    statistics.stable =
        statistics.jitterUs <=
        Config::StableSpreadUs;

    statistics.sampleCount = 0;

    sum = 0;

    minimum =
        std::numeric_limits<uint16_t>::max();

    maximum = 0;
}

Data data()
{
    return statistics;
}

}