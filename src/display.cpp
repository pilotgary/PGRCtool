/**
 * @file display.cpp
 * @brief SH1106 OLED display implementation.
 */

#include "display.h"

#include "config.h"
#include "version.h"

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

namespace
{

    U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(
        U8G2_R0,
        U8X8_PIN_NONE);

    Display::Data displayData{};

    constexpr uint8_t DisplayWidth  = 128;
    constexpr uint8_t DisplayHeight = 64;

    void drawScopeWaveform();
    void drawGeneratorWaveform();
    void drawScopeScreen();
    void drawGeneratorScreen();
    void drawStatisticsScreen();

    struct Waveform
    {
        uint32_t displaySpanUs;

        uint32_t offsetUs;

        uint16_t pulseWidthUs;

        const char* scaleLabel;
    };

//    void drawWaveform(const Waveform&);

} // anonymous namespace


namespace Display
{

    bool init()
    {
        Wire.begin(
            Config::OledSdaPin,
            Config::OledSclPin);

        oled.setI2CAddress(
            Config::OledAddress << 1);

        return oled.begin();
    }

    void setData(const Data& data)
    {
        displayData = data;
    }

    void splash()
    {
        constexpr int InstructionStartY = 12;
        constexpr int InstructionLineSpacing = 12;

        oled.clearBuffer();

        oled.setFont(u8g2_font_ncenB10_tr);

        const char* const instructions[]
        {
            "SparkyFlight-S3",
            "Button clicks:",
            "Single: Loop PWM",
            "Double: Next Function",
            "Long: Change Mode"
        };

         for (uint8_t index = 0;
             index < sizeof(instructions) / sizeof(instructions[0]);
             ++index)
        {
            const int width = oled.getStrWidth(instructions[index]);

            oled.drawStr(
                (DisplayWidth - width) / 2,
                InstructionStartY + index * InstructionLineSpacing + (index == 0 ? 0 : 2),
                instructions[index]);

            oled.setFont(u8g2_font_6x10_tf);

        }

        oled.sendBuffer();

        delay(4000);
    }


    void refresh()
    {
        oled.clearBuffer();

        oled.setFont(u8g2_font_6x10_tf);

        switch (displayData.mode)
        {
            case App::Mode::GeneratorView:
                drawGeneratorScreen();
                break;

            case App::Mode::ScopeView:
                drawScopeScreen();
                break;

            case App::Mode::StatisticsView:
                drawStatisticsScreen();
                break;
        }

        oled.sendBuffer();
    }

} // namespace Display

namespace
{

    void drawWaveform(uint32_t inputPulseUs, bool signalPresent)
    {
        constexpr int Left = 4;
        constexpr int Right = 127;
        constexpr int Top = 25;
        constexpr int Bottom = 52;

        constexpr int TimebaseUsPerPixel = 50;

        oled.drawStr(2, 63, "50us/Pixel");

        if (!signalPresent)
        {
            oled.drawStr(38, 43, "NO SIGNAL");
            oled.drawHLine(Left, Bottom, Right - Left);
            return;
        }

        const int risingX = Left + 2;

        const int highPixels =
            constrain(
                (inputPulseUs + TimebaseUsPerPixel / 2) /
                TimebaseUsPerPixel,
                1,
                Right - risingX);

        const int fallingX = risingX + highPixels;

        oled.drawHLine(Left, Bottom, risingX - Left);
        oled.drawVLine(risingX, Top, Bottom - Top + 1);
        oled.drawHLine(risingX, Top, highPixels);
        oled.drawVLine(fallingX, Top, Bottom - Top + 1);
        oled.drawHLine(fallingX, Bottom, Right - fallingX);
    }
    
    void drawGeneratorScreen()
    {
        char line[24] = "GENERATOR";

        int width = oled.getStrWidth(line);

        oled.drawStr( (DisplayWidth - width), 63, line);

        const char* pulseLabel =
            (displayData.generatorMode == Generator::GeneratorMode::Manual)
            ? "PWM Value"
            : "PWM Sweep";

        const char* modeLabel = "Manual";

        switch (displayData.generatorMode)
        {
            case Generator::GeneratorMode::Sine:
                modeLabel = "Sinusoidal";
                break;

            case Generator::GeneratorMode::Triangular:
                modeLabel = "Triangular";
                break;

            case Generator::GeneratorMode::Square:
                modeLabel = "Squarewave";
                break;

            case Generator::GeneratorMode::Manual:
                break;
        }

        snprintf(
            line,
            sizeof(line),
            "%s : %4u us",
            pulseLabel,
            displayData.outputPulseUs);

        oled.drawStr(
            0,
            10,
            line);

        snprintf(
            line,
            sizeof(line),
            "Mode : %s",
            modeLabel);

        oled.drawStr(
            0,
            22,
            line);

        drawWaveform(displayData.outputPulseUs, true);
    }

    void drawScopeScreen()
    {
        char line[24] = "SCOPE";

        int width = oled.getStrWidth(line);

        oled.drawStr( (DisplayWidth - width), 63, line);

        if (displayData.signalPresent)
        {
            if (displayData.stable)
            {
                snprintf(
                    line,
                    sizeof(line),
                    "PWM/Jitter: %4u/%u us",
                    displayData.stablePulseUs,
                    displayData.jitterUs );

                oled.drawStr(0, 10, line);
            }
            else
            {
                snprintf(
                    line,
                    sizeof(line),
                    "PWM/Jitter: %4u/0 us",
                    displayData.inputPulseUs);

                oled.drawStr(0, 10, line);
            }
            snprintf(
                line,
                sizeof(line),
                "Rate      : %6u Hz",
                displayData.frameRateHz);

            oled.drawStr(0, 22, line);
                  
        }
        else
        {
            oled.drawStr(
                0,
                22,
                "Waiting for PWM...");
        }

        drawWaveform(displayData.inputPulseUs, displayData.signalPresent);
    }

    void drawStatisticsScreen()
    {
        char line[24];

        oled.drawStr(
            0,
            10,
            "PWM STATISTICS");

        snprintf(
            line,
            sizeof(line),
            "Average : %4u us",
            displayData.stablePulseUs);

        oled.drawStr(0,22,line);

        snprintf(
            line,
            sizeof(line),
            "Peak-Pk : %4u us",
            displayData.peakToPeakUs);

        oled.drawStr(0,34,line);

        snprintf(
            line,
            sizeof(line),
            "Min/Max : %4u/%4u",
            displayData.minimumPulseUs,
            displayData.maximumPulseUs);

        oled.drawStr(0,46,line);

        snprintf(
            line,
            sizeof(line),
            "Samples : %lu",
            (unsigned long)displayData.totalSamples);

        oled.drawStr(0,58,line);
    }
}