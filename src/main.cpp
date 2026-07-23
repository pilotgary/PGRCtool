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
#include "button.h"
//#include "menu.h"
#include "statistics.h"
#include "app.h"
#include "version.h"

namespace
{
    App::Mode currentMode = App::Mode::GeneratorView;
}

void setup()
{   
    //delay(5000);
    //Serial.begin(115200);

    Hardware::init();
    Button::init();
    Display::init();
    Generator::init();
    Capture::init();
    Statistics::init();

    Display::splash();
}


void loop()
{
    Button::update();

    switch (Button::getEvent())
    {
        case Button::Event::ShortPress:

            if (currentMode == App::Mode::GeneratorView)
            {
                Generator::nextPreset();
            }

            break;

        case Button::Event::LongPress:

            currentMode =
                (currentMode == App::Mode::ScopeView)
                ? App::Mode::GeneratorView
                : App::Mode::ScopeView;

            break;

        default:
            break;
    }

    Generator::update();

    Capture::update();

    const auto measurement = Capture::getMeasurement();

    Statistics::update(measurement);

    const auto stats = Statistics::data();

    Display::Data display{};

    display.mode = currentMode;

    display.signalPresent     = stats.signalPresent;
    display.stable            = stats.stable;
    display.inputPulseUs      = stats.pulseWidthUs;
    display.stablePulseUs     = stats.averagePulseUs;
    display.acquiringSamples  = stats.sampleCount;
    display.outputPulseUs     = Generator::outputPulseUs();
    display.framePeriodUs     = stats.framePeriodUs;
    display.frameRateHz       = stats.frameRateHz;
    display.jitterUs          = stats.jitterUs;

    Display::setData(display);

    Display::refresh();
}

