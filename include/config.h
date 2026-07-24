#pragma once

/**
 * @file config.h
 * @brief Global project configuration.
 */

#include <stdint.h>

namespace Config
{
    //--------------------------------------------------
    // GPIO
    //--------------------------------------------------

    constexpr uint8_t OledSclPin = 1;
    constexpr uint8_t OledSdaPin = 2;

    constexpr uint8_t PwmInputPin = 4;
    constexpr uint8_t PwmOutputPin = 5;

    constexpr uint8_t ButtonPin = 6;

    //--------------------------------------------------
    // OLED
    //--------------------------------------------------

    constexpr uint8_t OledAddress = 0x3C;

    constexpr uint16_t DisplayWidth = 128;
    constexpr uint16_t DisplayHeight = 64;

    //--------------------------------------------------
    // Generator
    //--------------------------------------------------

    constexpr uint32_t ServoFrameUs = 20000;

    constexpr uint16_t ServoMinUs = 1000;
    constexpr uint16_t ServoCentreUs = 1500;
    constexpr uint16_t ServoMaxUs = 2000;

    constexpr uint16_t ServoExtendedMinUs = 500;
    constexpr uint16_t ServoExtendedMaxUs = 2500;

    constexpr uint32_t PwmFrequencyHz    = 50;
    constexpr uint8_t  PwmResolutionBits = 14;

    //--------------------------------------------------
    // Capture
    //--------------------------------------------------

    constexpr uint32_t SignalTimeoutUs = 100000;

    constexpr uint8_t StableSamples = 16;

    constexpr uint16_t StableSpreadUs = 8;
}