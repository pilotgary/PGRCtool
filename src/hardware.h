#pragma once

/**
 * @file hardware.h
 * @brief Hardware Abstraction Layer.
 */

#include <Arduino.h>

namespace Hardware
{
    /**
     * Initialise board hardware.
     *
     * Configures GPIO, timers, PWM and I²C.
     *
     * @return true if successful.
     */
    bool init();

    /**
     * Millisecond system timer.
     */
    uint32_t millis();

    /**
     * Microsecond system timer.
     */
    uint32_t micros();

    /**
     * Read the front-panel button.
     *
     * @return true while pressed.
     */
    bool buttonPressed();

    /**
     * Set servo output pulse width.
     *
     * @param pulseWidthUs Pulse width in microseconds.
     */
    void setServoPulse(uint16_t pulseWidthUs);
}