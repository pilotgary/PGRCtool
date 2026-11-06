# PGRCTool

**RC PWM Signal Analyzer & Generator for the ESP32-S3**

PGRCTool is a compact handheld tool for generating, viewing and analysing hobby RC PWM servo signals using an ESP32-S3 and a 128×64 OLED display.

Designed primarily for RC aircraft, robotics and embedded development, it combines a precision servo signal generator with a real-time PWM analyser in a simple single-button user interface.

---

## Features

### Servo Signal Generator

- Standard 50 Hz RC servo output
- Preset pulse widths
- Continuous waveform generation
- Multiple sweep modes:
  - Sine
  - Triangle
  - Square
- Smooth non-blocking waveform generation
- Output remains active while using other screens

### PWM Signal Analyzer

Measures incoming PWM signals including:

- Pulse width
- Frame period
- Frame rate
- Signal stability

### Statistics

Displays:

- Average pulse width
- Minimum pulse width
- Maximum pulse width
- Peak-to-peak variation
- Total captured samples

### Display

128 × 64 OLED user interface including:

- Generator screen
- Scope screen
- Statistics screen

---

## Hardware

Designed for an ESP32-S3 development board together with:

- SSD1306 128×64 I²C OLED
- One push button
- PWM input
- PWM output

**NOTE : The PWM input and PWM output pins on the ESP32-S3 are 3.3V rated.  Applying 5V to them could damage the ESP32-S3.  Use a 3.3V to 5V logic level level converter or a voltage divider. More details below.**

GPIO assignments are defined in:

```
include/config.h
```

Default assignments:

| Function | GPIO |
|----------|-----:|
| OLED SCL | 1 |
| OLED SDA | 2 |
| PWM Input | 4 |
| PWM Output | 5 |
| Push Button | 6 |

The PWM output can be looped back to the PWM input for self testing.  The signal generator keeps running in the last set mode while the screen is displaying the PWM input waveform.

---

## Wiring

![Wiring Diagram](images/wiring.jpg)

---

## User Interface

The entire application is controlled using a single push button.

### Generator Screen

Displays:

- Output pulse width
- Output frequency
- Current generator mode

Controls:

| Action | Function |
|--------|----------|
| Short Press | Next preset pulse width (Manual) or next sweep type (Sweep) |
| Double Click | Toggle Manual / Sweep mode |
| Long Press | Switch to Scope screen |

---

### Scope Screen

Displays:

- Measured pulse width
- Frame rate
- Captured waveform

Controls:

| Action | Function |
|--------|----------|
| Double Click | Statistics screen |
| Long Press | Generator screen |

---

### Statistics Screen

Displays:

- Average pulse width
- Peak-to-peak variation
- Minimum / Maximum pulse width
- Sample count

Controls:

| Action | Function |
|--------|----------|
| Double Click | Scope screen |
| Long Press | Generator screen |

---

## Project Structure

```
include/
    app.h
    config.h
    version.h

src/
    button.*
    capture.*
    display.*
    generator.*
    hardware.*
    statistics.*
    main.cpp
```

The project is deliberately modular.

| Module | Responsibility |
|---------|----------------|
| Hardware | GPIO, timers and peripherals |
| Button | Button event state machine |
| Generator | PWM output generation |
| Capture | PWM input measurement |
| Statistics | Signal statistics |
| Display | OLED rendering |
| Main | Application state machine |

---

## Software Architecture

The firmware is built around a non-blocking main loop.

Each module owns its own functionality:

- Button event handling
- PWM generation
- PWM capture
- Statistics
- Display rendering

No module performs work that belongs to another, making the code straightforward to extend and maintain.

---

## Building

The project uses **PlatformIO**.

```
pio run
```

Upload:

```
pio run --target upload
```

Monitor:

```
pio device monitor
```

---

## Future Development

Planned enhancements include:

- Additional waveform types
- User configurable sweep parameters
- Extended statistics
- Persistent settings
- Additional RC protocol tools

---

## License

This project is released under the MIT License.

---

## Acknowledgements

Developed using:

- ESP32-S3
- PlatformIO
- Arduino Framework
- SSD1306 OLED display library

---

## Version

Current Release Candidate:

**RC1**
