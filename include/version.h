#pragma once

/**
 * @file version.h
 * @brief SparkyFlight-S3 firmware version information.
 */

#include <stdint.h>

namespace Version
{
    constexpr uint8_t Major = 2;
    constexpr uint8_t Minor = 0;
    constexpr uint8_t Patch = 0;

    constexpr char Suffix[] = "dev2";

    constexpr char String[] = "2.0.0-dev2";
}