/**
 * @file capture.cpp
 * @brief PWM input capture.
 */

#include "capture.h"

#include "config.h"
#include "hardware.h"

#include <Arduino.h>
#include <stddef.h>

namespace
{

portMUX_TYPE captureLock = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t riseTimeUs = 0;
volatile uint32_t lastEdgeUs = 0;

volatile uint16_t latestPulseUs = 0;
volatile uint32_t pulseCounter = 0;

volatile uint32_t previousRiseTimeUs = 0;
volatile uint32_t latestFramePeriodUs = 0;
volatile uint16_t latestPulseWidthUs = 0;
volatile uint32_t latestSampleNumber = 0;

volatile bool newPulseAvailable = false;

Capture::Measurement currentMeasurement;

}

namespace
{

void IRAM_ATTR pwmEdge()
{
    const uint32_t now = Hardware::micros();

    const bool level =  digitalRead(Config::PwmInputPin);

    lastEdgeUs = now;

    if (level)
    {
        if (previousRiseTimeUs != 0)
        {
            latestFramePeriodUs = now - previousRiseTimeUs;
        }

        previousRiseTimeUs = now;

        riseTimeUs = now;

        return;
    }

    const uint32_t width =
        now - riseTimeUs;

    if (width < Config::ServoExtendedMinUs)
    {
        return;
    }

    if (width > Config::ServoExtendedMaxUs)
    {
        return;
    }

    portENTER_CRITICAL_ISR(&captureLock);

    latestPulseUs = static_cast<uint16_t>(width);

    ++pulseCounter;

    newPulseAvailable = true;

    portEXIT_CRITICAL_ISR(&captureLock);
}

}

namespace Capture
{

bool init()
{
    pinMode(
        Config::PwmInputPin,
        INPUT_PULLDOWN);

    attachInterrupt(
        digitalPinToInterrupt(
            Config::PwmInputPin),
        pwmEdge,
        CHANGE);

    currentMeasurement = {};

    return true;
}

void update()
{
    uint16_t pulse;
    uint32_t count;
    uint32_t edge;
    uint32_t frame;
    bool available;

    portENTER_CRITICAL(&captureLock);

    pulse     = latestPulseUs;
    count     = pulseCounter;
    edge      = lastEdgeUs;
    frame     = latestFramePeriodUs;
    available = newPulseAvailable;

    newPulseAvailable = false;

    portEXIT_CRITICAL(&captureLock);

    if (available)
    {
        currentMeasurement.pulseWidthUs  = pulse;
        currentMeasurement.sampleNumber  = count;
        currentMeasurement.framePeriodUs = frame;
    }

    currentMeasurement.signalPresent =
        (Hardware::micros() - edge) < Config::SignalTimeoutUs;
}

Measurement getMeasurement()
{
    return currentMeasurement;
}

}