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
        oled.clearBuffer();

        oled.setFont(u8g2_font_ncenB10_tr);

        const char title[] = "PWM Scope";

        const int titleWidth =
            oled.getStrWidth(title);

        oled.drawStr(
            (DisplayWidth - titleWidth) / 2,
            20,
            title);

        oled.setFont(u8g2_font_6x10_tf);

        const char line1[] = "Hold button 2s to";
        const char line2[] = "switch function";

        int width = oled.getStrWidth(line1);

        oled.drawStr(
            (DisplayWidth - width) / 2,
            40,
            line1);

        width = oled.getStrWidth(line2);

        oled.drawStr(
            (DisplayWidth - width) / 2,
            56,
            line2);

        oled.sendBuffer();

        delay(2000);
    }

    void refresh()
    {
        oled.clearBuffer();

        oled.setFont(u8g2_font_6x10_tf);

        switch (displayData.mode)
        {
            case App::Mode::Scope:
                drawScopeScreen();
                break;

            case App::Mode::Generator:
                drawGeneratorScreen();
                break;
        }

        oled.sendBuffer();
    }

} // namespace Display

namespace
{

    void drawScopeWaveform()
    {
        constexpr int Left = 4;
        constexpr int Top = 27;
        constexpr int Bottom = 52;

        constexpr int TimebaseUsPerPixel = 50;

        oled.drawFrame(0, Top - 2, 128, Bottom - Top + 5);

        for (int x = Left; x < 128; x += 20)
        {
            oled.drawVLine(x, Top, Bottom - Top);
        }

        oled.drawStr(2, 62, "50us/pixel");

        if (!displayData.signalPresent)
        {
            oled.drawStr(38, 43, "NO SIGNAL");
            return;
        }

        const int risingX = Left + 2;

        const int highPixels =
            constrain(
                (displayData.inputPulseUs + TimebaseUsPerPixel / 2) /
                TimebaseUsPerPixel,
                1,
                128 - risingX - 1);

        const int fallingX = risingX + highPixels;

        oled.drawHLine(Left, Bottom, risingX - Left);
        oled.drawVLine(risingX, Top, Bottom - Top + 1);
        oled.drawHLine(risingX, Top, highPixels);
        oled.drawVLine(fallingX, Top, Bottom - Top + 1);
        oled.drawHLine(fallingX, Bottom, 127 - fallingX);
    }

    void drawGeneratorWaveform()
    {
        constexpr int Left   = 4;
        constexpr int Right  = 127;
        constexpr int Top    = 27;
        constexpr int Bottom = 52;

        constexpr uint32_t OffsetUs      = 400;
        constexpr uint32_t DisplaySpanUs = Config::ServoFrameUs;

        const int drawWidth = Right - Left;

        const float usPerPixel =
            static_cast<float>(DisplaySpanUs) / drawWidth;

        oled.drawFrame(0, Top - 2, 128, Bottom - Top + 5);

        char label[24];

        snprintf(
            label,
            sizeof(label),
            "%.0fus/pixel",
            usPerPixel);

        oled.drawStr(2, 62, label);

        auto usToX = [&](uint32_t us)
        {
            int x = Left +
                static_cast<int>(us / usPerPixel + 0.5f);

            if (x < Left)
                x = Left;

            if (x > Right)
                x = Right;

            return x;
        };

        const int riseX =
            usToX(OffsetUs);

        const int fallX =
            usToX(
                OffsetUs +
                displayData.outputPulseUs);

        oled.drawHLine(
            Left,
            Bottom,
            riseX - Left);

        oled.drawVLine(
            riseX,
            Top,
            Bottom - Top + 1);

        oled.drawHLine(
            riseX,
            Top,
            fallX - riseX);

        oled.drawVLine(
            fallX,
            Top,
            Bottom - Top + 1);

        oled.drawHLine(
            fallX,
            Bottom,
            Right - fallX);
    }

    void drawScopeScreen()
    {
        char line[24];

        oled.drawStr(
            0,
            10,
            "PWM SCOPE  0-5V IN");

        if (displayData.signalPresent)
        {
            snprintf(
                line,
                sizeof(line),
                "Pulse: %4u us",
                displayData.inputPulseUs);

            oled.drawStr(0, 22, line);

            if (displayData.stable)
            {
                snprintf(
                    line,
                    sizeof(line),
                    "STABLE: %4u us",
                    displayData.stablePulseUs);
            }
            else
            {
                snprintf(
                    line,
                    sizeof(line),
                    "ACQUIRING (%u/16)",
                    displayData.acquiringSamples);
            }

            oled.drawStr(0, 34, line);
        }
        else
        {
            oled.drawStr(
                0,
                22,
                "Waiting for PWM...");
        }

        drawScopeWaveform();
    }

    void drawGeneratorScreen()
    {
        char line[24];

        oled.drawStr(
            0,
            10,
            "PWM GENERATOR");

        snprintf(
            line,
            sizeof(line),
            "Output: %4u us",
            displayData.outputPulseUs);

        oled.drawStr(
            0,
            22,
            line);

        oled.drawStr(
            0,
            34,
            "Hold 2s: to SCOPE");

        drawGeneratorWaveform();
    }

}