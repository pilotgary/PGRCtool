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
        ScopeView,
        GeneratorView,
        StatisticsView
    };

    Mode currentMode();

    void setMode(Mode mode);

}