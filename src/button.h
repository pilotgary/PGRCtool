#pragma once

/**
 * @file button.h
 * @brief Push button interface.
 */

#include <stdint.h>

namespace Button
{

enum class Event : uint8_t
{
    None,
    ShortPress,
    DoubleClick,
    LongPress
};

/**
 * @brief Initialise the push button.
 *
 * @return true if successful.
 */
bool init();

/**
 * @brief Update the button state machine.
 *
 * Call once each pass through loop().
 */
void update();

/**
 * @brief Return the next queued button event.
 *
 * Reading an event removes it from the queue.
 *
 * @return Event::None if no events are pending.
 */
Event readEvent();

} // namespace Button