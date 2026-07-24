#include "statistics.h"

#include "config.h"

#include <Arduino.h>

#include <limits>

namespace
{

constexpr uint32_t PublishIntervalMs = 250;

Statistics::Data statistics;

uint32_t lastSampleNumber = UINT32_MAX;
uint32_t lastPublishMs = 0;

uint64_t accumulatedPulseUs = 0;
uint32_t accumulatedSamples = 0;
uint16_t accumulatedMinimumUs = UINT16_MAX;
uint16_t accumulatedMaximumUs = 0;

bool newDataReady = false;

void publishData(uint32_t now)
{
    if ((now - lastPublishMs) < PublishIntervalMs)
    {
        return;
    }

    lastPublishMs = now;

    statistics.totalSamples = accumulatedSamples;

    if (accumulatedSamples == 0)
    {
        statistics.averagePulseUs = 0;
        statistics.minimumPulseUs = 0;
        statistics.maximumPulseUs = 0;
        statistics.peakToPeakUs = 0;
        statistics.jitterUs = 0;
        statistics.sampleCount = 0;
        statistics.stable = false;
    }
    else
    {
        statistics.averagePulseUs =
            static_cast<uint16_t>(
                accumulatedPulseUs / accumulatedSamples);
        statistics.minimumPulseUs = accumulatedMinimumUs;
        statistics.maximumPulseUs = accumulatedMaximumUs;
        statistics.peakToPeakUs =
            accumulatedMaximumUs - accumulatedMinimumUs;
        statistics.jitterUs = statistics.peakToPeakUs;
        statistics.sampleCount =
            (accumulatedSamples > UINT8_MAX)
            ? UINT8_MAX
            : static_cast<uint8_t>(accumulatedSamples);
        statistics.stable =
            statistics.peakToPeakUs <= Config::StableSpreadUs;
    }

    newDataReady = true;
}

} // anonymous namespace

namespace Statistics
{

void init()
{
    reset();
}

void reset()
{
    statistics = {};

    lastSampleNumber = UINT32_MAX;
    lastPublishMs = millis();

    accumulatedPulseUs = 0;
    accumulatedSamples = 0;
    accumulatedMinimumUs = UINT16_MAX;
    accumulatedMaximumUs = 0;

    newDataReady = false;
}

void update(const Capture::Measurement& measurement)
{
    const uint32_t now = millis();

    statistics.signalPresent = measurement.signalPresent;

    if (measurement.sampleNumber != lastSampleNumber)
    {
        lastSampleNumber = measurement.sampleNumber;

        statistics.pulseWidthUs = measurement.pulseWidthUs;
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

        if (measurement.signalPresent)
        {
            ++accumulatedSamples;
            accumulatedPulseUs += measurement.pulseWidthUs;

            if (measurement.pulseWidthUs < accumulatedMinimumUs)
            {
                accumulatedMinimumUs = measurement.pulseWidthUs;
            }

            if (measurement.pulseWidthUs > accumulatedMaximumUs)
            {
                accumulatedMaximumUs = measurement.pulseWidthUs;
            }
        }
    }

    publishData(now);
}

bool readData(Data& data)
{
    if (!newDataReady)
    {
        return false;
    }

    data = statistics;
    newDataReady = false;

    return true;
}

Data data()
{
    return statistics;
}

} // namespace Statistics