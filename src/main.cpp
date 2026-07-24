/**
 * @file main.cpp
 * @brief SparkyFlight-PAG application entry point.
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
    App::Mode previousMode = App::Mode::GeneratorView;
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

    switch (Button::readEvent())
    {
        case Button::Event::ShortPress:

            if (currentMode == App::Mode::GeneratorView)
            {
                if (Generator::mode() == Generator::GeneratorMode::Manual)
                {
                    Generator::nextPreset();
                }
                else
                {
                    Generator::nextSweepMode();
                }
            }

            break;

        case Button::Event::DoubleClick:

            if (currentMode == App::Mode::GeneratorView)
            {
                Generator::toggleMode();
                break;
            }
            switch (currentMode)
            {
                case App::Mode::ScopeView:
                    currentMode = App::Mode::StatisticsView;
                    break;

                case App::Mode::StatisticsView:
                    currentMode = App::Mode::ScopeView;
                    break;

                default:
                    break;
            }

            break;

        case Button::Event::LongPress:

            switch (currentMode)
            {
                case App::Mode::GeneratorView:
                    currentMode = App::Mode::ScopeView;
                    break;

                default:
                    currentMode = App::Mode::GeneratorView;
                    break;
            }

            break;

        default:
            break;
    }

    if (currentMode == App::Mode::ScopeView &&
        previousMode != App::Mode::ScopeView)
    {
        Statistics::reset();
    }

    previousMode = currentMode;

    Generator::update();

    Capture::update();

    const auto measurement = Capture::getMeasurement();

    Statistics::update(measurement);

    Statistics::Data stats = Statistics::data();

    if (currentMode == App::Mode::ScopeView)
    {
        Statistics::Data newStatistics;

        if (Statistics::readData(newStatistics))
        {
            stats = newStatistics;
        }
    }

    Display::Data display{};

    display.mode = currentMode;

    display.signalPresent     = stats.signalPresent;
    display.stable            = stats.stable;
    display.inputPulseUs      = stats.pulseWidthUs;
    display.stablePulseUs     = stats.averagePulseUs;
    display.acquiringSamples  = stats.sampleCount;
    display.outputPulseUs     = Generator::outputPulseUs();
    display.generatorMode     = Generator::mode();
    display.framePeriodUs     = stats.framePeriodUs;
    display.frameRateHz       = stats.frameRateHz;
    display.jitterUs          = stats.jitterUs;
    display.minimumPulseUs    = stats.minimumPulseUs;
    display.maximumPulseUs    = stats.maximumPulseUs;
    display.peakToPeakUs      = stats.peakToPeakUs;
    display.totalSamples      = stats.totalSamples;

    Display::setData(display);

    Display::refresh();
}

