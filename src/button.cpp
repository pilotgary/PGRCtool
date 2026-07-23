/**
 * @file button.cpp
 * @brief Push button implementation.
 */

#include "button.h"

#include "config.h"

#include <Arduino.h>

namespace
{
    constexpr uint32_t DebounceTimeMs = 30;
    constexpr uint32_t LongPressTimeMs = 2000;

    bool lastRawState      = false;
    bool stableState       = false;

    uint32_t lastChangeMs  = 0;
    uint32_t pressStartMs  = 0;

    bool longPressSent     = false;

    Button::Event pendingEvent =
        Button::Event::None;
}

namespace Button
{

bool init()
{
    pinMode(
        Config::ButtonPin,
        INPUT_PULLUP);

    const bool pressed =
        (digitalRead(Config::ButtonPin) == LOW);

    lastRawState = pressed;
    stableState  = pressed;

    lastChangeMs = millis();
    pressStartMs = 0;

    longPressSent = false;
    pendingEvent = Event::None;

    return true;
}

void update()
{
    const uint32_t now = millis();

    const bool raw =
        (digitalRead(Config::ButtonPin) == LOW);

    //
    // Debounce
    //

    if (raw != lastRawState)
    {
        lastRawState = raw;
        lastChangeMs = now;
    }

    if ((now - lastChangeMs) >= DebounceTimeMs &&
        raw != stableState)
    {
        stableState = raw;

        if (stableState)
        {
            //
            // Button pressed
            //

            pressStartMs = now;
            longPressSent = false;
        }
        else
        {
            //
            // Button released
            //

            if (!longPressSent)
            {
                pendingEvent = Event::ShortPress;
            }
        }
    }

    //
    // Long press detection
    //

    if (stableState &&
        !longPressSent &&
        (now - pressStartMs) >= LongPressTimeMs)
    {
        longPressSent = true;

        pendingEvent = Event::LongPress;
    }
}

Event getEvent()
{
    Event event = pendingEvent;

    pendingEvent = Event::None;

    return event;
}

}   // namespace Button