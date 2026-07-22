/**
 * @file hardware.cpp
 * @brief Hardware Abstraction Layer implementation.
 */

#include "hardware.h"
#include "config.h"

#include <Arduino.h>

namespace
{
    //----------------------------------------------------------------------
    // Private constants
    //----------------------------------------------------------------------

    constexpr uint8_t LedcChannel = 0;

    //----------------------------------------------------------------------
    // Private functions
    //----------------------------------------------------------------------

    void initialisePins()
    {
        pinMode(Config::ButtonPin, INPUT_PULLUP);
        pinMode(Config::PwmOutputPin, OUTPUT);
    }

    //------------------------------------------------------------------

    bool initialisePwm()
    {
#if ESP_ARDUINO_VERSION_MAJOR >= 3

        if (!ledcAttach(Config::PwmOutputPin,
                        Config::PwmFrequencyHz,
                        Config::PwmResolutionBits))
        {
            return false;
        }

#else

        ledcSetup(
            LedcChannel,
            Config::PwmFrequencyHz,
            Config::PwmResolutionBits);

        ledcAttachPin(
            Config::PwmOutputPin,
            LedcChannel);

#endif

        return true;
    }

    //------------------------------------------------------------------

    uint32_t pulseWidthToDuty(uint16_t pulseWidthUs)
    {
        const uint32_t maxDuty =
            (1UL << Config::PwmResolutionBits) - 1;

        const uint64_t numerator =
            static_cast<uint64_t>(pulseWidthUs) * maxDuty;

        return static_cast<uint32_t>(
            numerator / Config::ServoFrameUs);
    }

} // anonymous namespace

//======================================================================
// Public interface
//======================================================================

namespace Hardware
{

bool init()
{
    initialisePins();

    if (!initialisePwm())
    {
        return false;
    }

    setServoPulse(Config::ServoCentreUs);

    return true;
}

//------------------------------------------------------------------

uint32_t millis()
{
    return ::millis();
}

//------------------------------------------------------------------

uint32_t micros()
{
    return ::micros();
}

//------------------------------------------------------------------

bool buttonPressed()
{
    return digitalRead(Config::ButtonPin) == LOW;
}

//------------------------------------------------------------------

void setServoPulse(uint16_t pulseWidthUs)
{
    if (pulseWidthUs < Config::ServoExtendedMinUs)
    {
        pulseWidthUs = Config::ServoExtendedMinUs;
    }

    if (pulseWidthUs > Config::ServoExtendedMaxUs)
    {
        pulseWidthUs = Config::ServoExtendedMaxUs;
    }

    const uint32_t duty = pulseWidthToDuty(pulseWidthUs);

#if ESP_ARDUINO_VERSION_MAJOR >= 3

    ledcWrite(Config::PwmOutputPin, duty);

#else

    ledcWrite(LedcChannel, duty);

#endif
}

} // namespace Hardware