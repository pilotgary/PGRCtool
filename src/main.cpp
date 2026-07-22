/**
 * @file main.cpp
 * @brief SparkyFlight-S3 application entry point.
 *
 * Initialises all modules and executes the main application loop.
 * This file contains only application orchestration and state
 * management. Hardware access, display rendering and signal
 * processing are delegated to their respective modules.
 */

#include <Arduino.h>

#include "config.h"
#include "capture.h"
#include "display.h"
//#include "generator.h"
#include "hardware.h"
//#include "menu.h"
//#include "statistics.h"
#include "types.h"
#include "version.h"

namespace
{
    App::Mode currentMode = App::Mode::Scope;

    uint16_t outputPulseUs = Config::ServoCentreUs;

    void handleButton();
}
void setup()
{
    Hardware::init();

    Capture::init();

    Display::init();

    Display::splash();
}

void loop()
{
    Capture::update();

    const auto capture = Capture::getMeasurement();

    Display::Data display{};

    display.mode = currentMode;

    display.signalPresent = capture.signalPresent;

    display.inputPulseUs = capture.pulseWidthUs;

    display.outputPulseUs = outputPulseUs;

    // Temporary until Statistics module exists
    display.stable = false;
    display.stablePulseUs = 0;
    display.acquiringSamples = 0;

    Display::setData(display);

    Display::refresh();

    handleButton();

    Hardware::setServoPulse(outputPulseUs);
}

namespace
{

void handleButton()
{
    // TODO
}

}