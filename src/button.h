#pragma once

/**
 * @file button.h
 * @brief Push button handler.
 */

#include <stdint.h>

namespace Button
{

enum class Event : uint8_t
{
    None,
    ShortPress,
    LongPress,

    // Reserved for future use
    DoublePress,
    Repeat
};

bool init();

void update();

Event getEvent();

}   // namespace Button