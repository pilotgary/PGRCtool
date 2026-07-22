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
#include "generator.h"
#include "hardware.h"
//#include "menu.h"
#include "statistics.h"
#include "app.h"
#include "version.h"

namespace
{
    App::Mode currentMode;

    void handleButton();
}

void setup()
{
    Hardware::init();
    Display::init();
    Generator::init();
    Capture::init();
    Statistics::init();

    Display::splash();
}


void loop()
{
    handleButton();

    Capture::update();

    const auto measurement = Capture::getMeasurement();

    Statistics::update(measurement);

    const auto stats = Statistics::data();

    Display::Data display{};

    display.mode = currentMode;

    display.signalPresent     = stats.signalPresent;
    display.stable            = stats.stable;
    display.inputPulseUs      = stats.pulseWidthUs;
    display.stablePulseUs     = stats.stablePulseUs;
    display.acquiringSamples  = stats.sampleCount;
    display.outputPulseUs     = Generator::outputPulseUs();

    Display::setData(display);

    Display::refresh();
}

namespace
{

void handleButton()
{
    if (Hardware::buttonPressed())
    {
        Generator::nextPreset();
    }
}

}