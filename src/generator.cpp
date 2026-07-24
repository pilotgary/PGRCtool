#include "generator.h"

#include "hardware.h"

#include <Arduino.h>
#include <math.h>

namespace
{

constexpr uint16_t SWEEP_MIN_US    = 1000;
constexpr uint16_t SWEEP_MAX_US    = 2000;
constexpr uint32_t SWEEP_PERIOD_MS = 5000;

constexpr uint8_t SineTableSize = 100;
constexpr float SweepTwoPi = 6.28318530718f;

double sineTable[SineTableSize];

constexpr uint16_t FixedPresets[]
{
    1000,
    1500,
    2000
};

constexpr uint8_t FixedPresetCount =
    sizeof(FixedPresets) / sizeof(FixedPresets[0]);

Generator::GeneratorMode currentMode = Generator::GeneratorMode::Manual;
Generator::GeneratorMode recordedSweepMode = Generator::GeneratorMode::Sine;

uint8_t presetIndex = 1;

uint16_t currentPulse = FixedPresets[presetIndex];

uint32_t sweepStartMs = 0;
float sweepPhase = 0.0f;

void initialiseSineTable()
{
    for (uint8_t index = 0; index < SineTableSize; ++index)
    {
        const float phase =
            SweepTwoPi * static_cast<float>(index) /
            static_cast<float>(SineTableSize);

        sineTable[index] = sinf(phase);
    }
}

float sineWaveValue(float phase)
{
    const float tablePosition =
        phase * static_cast<float>(SineTableSize);

    const uint8_t index =
        static_cast<uint8_t>(tablePosition);

    const uint8_t nextIndex =
        (index + 1) % SineTableSize;

    const float interpolation =
        tablePosition - static_cast<float>(index);

    return static_cast<float>(
        sineTable[index] +
        (sineTable[nextIndex] - sineTable[index]) * interpolation);
}

float triangularWaveValue(float phase)
{
    return (phase < 0.5f)
        ? phase * 2.0f
        : (1.0f - phase) * 2.0f;
}

float squareWaveValue(float phase)
{
    return (phase < 0.5f) ? 0.0f : 1.0f;
}

float pulseToWaveValue()
{
    if (currentPulse <= SWEEP_MIN_US)
    {
        return 0.0f;
    }

    if (currentPulse >= SWEEP_MAX_US)
    {
        return 1.0f;
    }

    return static_cast<float>(currentPulse - SWEEP_MIN_US) /
        static_cast<float>(SWEEP_MAX_US - SWEEP_MIN_US);
}

void setSweepPhase(float phase)
{
    sweepPhase = phase;

    sweepStartMs = millis() -
        static_cast<uint32_t>(
            sweepPhase * static_cast<float>(SWEEP_PERIOD_MS));
}

float risingSinePhase(float waveValue)
{
    float angle = asinf(waveValue * 2.0f - 1.0f);

    if (angle < 0.0f)
    {
        angle += SweepTwoPi;
    }

    return angle / SweepTwoPi;
}

} // anonymous namespace

namespace Generator
{

bool init()
{
    initialiseSineTable();

    recordedSweepMode = GeneratorMode::Sine;

    Hardware::setServoPulse(currentPulse);

    return true;
}

void update()
{
    if (currentMode == GeneratorMode::Manual)
    {
        return;
    }

    const uint32_t elapsedMs =
        (millis() - sweepStartMs) % SWEEP_PERIOD_MS;

    sweepPhase =
        static_cast<float>(elapsedMs) /
        static_cast<float>(SWEEP_PERIOD_MS);

    float waveformValue = 0.0f;

    switch (currentMode)
    {
        case GeneratorMode::Sine:
            waveformValue = (sineWaveValue(sweepPhase) + 1.0f) / 2.0f;
            break;

        case GeneratorMode::Triangular:
            waveformValue = triangularWaveValue(sweepPhase);
            break;

        case GeneratorMode::Square:
            waveformValue = squareWaveValue(sweepPhase);
            break;

        case GeneratorMode::Manual:
            return;
    }

    const float sweepPulse =
        static_cast<float>(SWEEP_MIN_US) +
        waveformValue *
        static_cast<float>(SWEEP_MAX_US - SWEEP_MIN_US);

    currentPulse =
        static_cast<uint16_t>(sweepPulse + 0.5f);

    Hardware::setServoPulse(currentPulse);
}

void nextPreset()
{
    if (currentMode != GeneratorMode::Manual)
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

void nextSweepMode()
{
    const float waveValue = pulseToWaveValue();

    switch (currentMode)
    {
        case GeneratorMode::Sine:
        {
            const bool rising =
                sweepPhase < 0.25f || sweepPhase >= 0.75f;

            setSweepPhase(
                rising
                ? waveValue / 2.0f
                : 1.0f - waveValue / 2.0f);

            currentMode = GeneratorMode::Triangular;
            break;
        }

        case GeneratorMode::Triangular:
            setSweepPhase(sweepPhase < 0.5f ? 0.5f : 0.0f);
            currentMode = GeneratorMode::Square;
            break;

        case GeneratorMode::Square:
            setSweepPhase(
                waveValue >= 0.5f
                ? 0.25f
                : 0.75f);

            currentMode = GeneratorMode::Sine;
            break;

        case GeneratorMode::Manual:
            break;
    }
}

void toggleMode()
{
    if (currentMode == GeneratorMode::Manual)
    {
        const float waveValue = pulseToWaveValue();

        currentMode = recordedSweepMode;

        switch (currentMode)
        {
            case GeneratorMode::Sine:
                setSweepPhase(risingSinePhase(waveValue));
                break;

            case GeneratorMode::Triangular:
                setSweepPhase(waveValue / 2.0f);
                break;

            case GeneratorMode::Square:
                setSweepPhase(waveValue >= 0.5f ? 0.5f : 0.0f);
                break;

            case GeneratorMode::Manual:
                break;
        }

        return;
    }

    recordedSweepMode = currentMode;
    currentMode = GeneratorMode::Manual;
}

uint16_t outputPulseUs()
{
    return currentPulse;
}

GeneratorMode mode()
{
    return currentMode;
}

} // namespace Generator