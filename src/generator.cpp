#include <iterator>
#include "generator.h"
#include "hardware.h"

namespace
{

    constexpr uint16_t FixedPresets[]
    {
        1000,
        1500,
        2000
    };

    constexpr uint8_t FixedPresetCount =
        sizeof(FixedPresets) / sizeof(FixedPresets[0]);

    Generator::Mode currentMode = Generator::Mode::Fixed;

    uint8_t presetIndex = 1;

    uint16_t currentPulse = FixedPresets[presetIndex];

    bool sweepIncreasing = true;

    uint32_t lastUpdateMs = 0;

}

namespace Generator
{

bool init()
{
    Hardware::setServoPulse(currentPulse);

    return true;
}

void update()
{
    switch (currentMode)
    {
        case Mode::Fixed:
            break;

        case Mode::Sweep:
            // To be implemented.
            break;

        case Mode::Sine:
            // To be implemented.
            break;
    }
}

void nextPreset()
{
    if (currentMode != Mode::Fixed)
    {
        return;
    }

    ++presetIndex;

   if (presetIndex >= FixedPresetCount)
    {
        presetIndex = 0;
    }    
    
    currentPulse = FixedPresets[presetIndex];

    Hardware::setServoPulse(currentPulse);
}

void nextMode()
{
    switch (currentMode)
    {
        case Mode::Fixed:
            currentMode = Mode::Sweep;
            break;

        case Mode::Sweep:
            currentMode = Mode::Sine;
            break;

        case Mode::Sine:
            currentMode = Mode::Fixed;
            break;
    }
}

uint16_t outputPulseUs()
{
    return currentPulse;
}

Mode mode()
{
    return currentMode;
}

} // namespace Generator