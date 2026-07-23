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

        const char title[] = "SparkyFlight-S3";

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
            case App::Mode::ScopeView:
                drawScopeScreen();
                break;

            case App::Mode::GeneratorView:
                drawGeneratorScreen();
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

        drawWaveform(displayData.outputPulseUs, true);
    }

    void drawScopeScreen()
    {
        char line[24] = "0-5V in";

        int width = oled.getStrWidth(line);

        oled.drawStr( (DisplayWidth - width), 63, line);

        if (displayData.signalPresent)
        {
            if (displayData.stable)
            {
                snprintf(
                    line,
                    sizeof(line),
                    "PWM/Jitter %4u/%u us",
                    displayData.stablePulseUs,
                    displayData.jitterUs );

                oled.drawStr(0, 10, line);

                snprintf(
                    line,
                    sizeof(line),
                    "Rate       %6u Hz",
                    displayData.frameRateHz);

                oled.drawStr(0, 22, line);
            }
            else
            {
                snprintf(
                    line,
                    sizeof(line),
                    "PWM/Jitter %4u/0 us",
                    displayData.inputPulseUs);

                oled.drawStr(0, 10, line);

                snprintf(
                    line,
                    sizeof(line),
                    "Rate       %6u Hz",
                    displayData.frameRateHz);

                oled.drawStr(0, 22, line);

                snprintf(
                    line,
                    sizeof(line),
                    "           Acquiring");

                oled.drawStr(0, 34, line);
            }        
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

}