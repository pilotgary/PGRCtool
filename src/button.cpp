/**
 * @file button.cpp
 * @brief Push button state machine.
 */

#include "button.h"

#include "config.h"

#include <Arduino.h>

namespace
{

constexpr uint32_t DebounceTimeMs    = 30;
constexpr uint32_t DoubleClickTimeMs = 300;
constexpr uint32_t LongPressTimeMs   = 2000;

constexpr uint8_t EventQueueSize = 4;

enum class State : uint8_t
{
    Idle,
    Pressed1,
    WaitSecond,
    Pressed2,
    WaitRelease
};

State state = State::Idle;

Button::Event eventQueue[EventQueueSize];

uint8_t queueHead = 0;
uint8_t queueTail = 0;
uint8_t queueCount = 0;

bool rawState = HIGH;
bool stableState = HIGH;

uint32_t debounceStart = 0;
uint32_t stateStart = 0;

inline bool pressed()
{
    return stableState == LOW;
}

inline bool released()
{
    return stableState == HIGH;
}

void queueEvent(Button::Event event)
{
    if (queueCount == EventQueueSize)
    {
        // Queue full.
        return;
    }

    eventQueue[queueHead] = event;
    queueHead = (queueHead + 1) % EventQueueSize;
    ++queueCount;
}

void debounceButton()
{
    const uint32_t now = millis();

    bool raw = digitalRead(Config::ButtonPin);

    if (raw != rawState)
    {
        rawState = raw;
        debounceStart = now;
    }

    if ((now - debounceStart) >= DebounceTimeMs)
    {
        stableState = rawState;
    }
}

} // anonymous namespace

namespace Button
{

bool init()
{
    pinMode(
        Config::ButtonPin,
        INPUT_PULLUP);

    rawState =
        digitalRead(Config::ButtonPin);

    stableState = rawState;

    queueHead = 0;
    queueTail = 0;
    queueCount = 0;

    state = State::Idle;

    return true;
}

void update()
{
    debounceButton();

    const uint32_t now = millis();

    switch (state)
    {
        case State::Idle:

            if (pressed())
            {
                state = State::Pressed1;
                stateStart = now;
            }

            break;

        case State::Pressed1:

            if ((now - stateStart) >= LongPressTimeMs)
            {
                queueEvent(Event::LongPress);
                state = State::WaitRelease;
            }
            else if (released())
            {
                state = State::WaitSecond;
                stateStart = now;
            }

            break;

        case State::WaitSecond:

            if (pressed())
            {
                state = State::Pressed2;
            }
            else if ((now - stateStart) >= DoubleClickTimeMs)
            {
                queueEvent(Event::ShortPress);
                state = State::Idle;
            }

            break;

        case State::Pressed2:

            if (released())
            {
                queueEvent(Event::DoubleClick);
                state = State::Idle;
            }

            break;

        case State::WaitRelease:

            if (released())
            {
                state = State::Idle;
            }

            break;
    }
}

Event readEvent()
{
    if (queueCount == 0)
    {
        return Event::None;
    }

    Event event = eventQueue[queueTail];

    queueTail =
        (queueTail + 1) % EventQueueSize;
    --queueCount;

    return event;
}

} // namespace Button