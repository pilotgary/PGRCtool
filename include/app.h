#pragma once

/**
 * @file app.h
 * @brief Shared data structures.
 */

#include <stdint.h>


namespace App
{

    enum class Mode : uint8_t
    {
        Scope,
        Generator
    };

}