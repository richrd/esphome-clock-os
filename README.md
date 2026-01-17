# ESPHome ClockOS

ESPHome ClockOS is my work in progress project for an ESPHome based desk clock with an OLED screen and rotary encoder. It is not only a clock and takes inspiration from 2000s era mobile phones.

![Clocks](assets/clocks.jpg)


### Contents
- [Hardware](#hardware)
- [Software](#software)
- [Features](#features)
- [Credits](#credits)


## Hardware
I'm building my devices with a "freeform circuits" approach using 1.5mm copper wire for the frames and 1mm wire for the connections to the display, rotary encoder etc, but any other method works too as long as the minimum hardware requirements are met. So far I've had a good time with ESP32 C3 Super Mini boards due to their tiny form factor.

Here are the hardware components used, the required ones are marked with a checkmark.

- [X] ESP32 (I'm using C3 but any ESP32 should work)
- [X] Screen (128x64)
    - Tested SSD1306 and SSD1309
- [X] Rotary encoder
- [ ] Beeper
- [ ] Vibration motor
- [ ] Joystick
- [ ] A / B buttons

## Software
ClockOS is built on the ESPHome platform.

## Features
- Clock, alarm and stopwatch
- Games (Breakout and Snake)
- Demos (examples of what can be done with graphics)
- Floorplan (for controlling entities in the house)
- Settings
- Display timeout

### Menu system
The default screen is the Clock, and clicking the encoder opens the main menu, below is the menu hierarchy which outlines the current features of the project:

    [ Main Menu ]
    ├── Clock ⏵
    │   ├── Default clock
    │   ├── Analog clock
    │   ├── Alarm ⏵
    │   │   ├── Alarm [OFF]
    │   │   ├── Hour [9.0]
    │   │   └── Minute [30.0]
    │   └── Stopwatch
    ├── Games ⏵
    │   ├── Autoplay [OFF]
    │   └── Demos ⏵
    │   │   ├── DVD (bouncing DVD logo screensaver)
    │   │   ├── Spin (spinning ball animation)
    │   │   ├── Cat & Yarn
    │   │   ├── Pinball (WIP) ⏵
    │   │   ├── Tokyo Bunny
    │   │   ├── Bunny Wiggle
    │   │   ├── Doodles
    │   │   ├── Text Input (WIP)
    │   │   ├── Joystick test
    │   │   └── Pixel paint
    │   ├── Breakout ⏵
    │   │   └── Play
    │   └── Snake ⏵
    │       ├── Play
    │       ├── Speed [7.0]
    │       └── Walls [ON]
    ├── Sounds ⏵
    │   ├── Mario
    │   ├── Star Wars
    │   ├── Mission Impossible
    │   ├── Barbie Girl
    │   └── ...
    ├── Control ⏵
    │   ├── Turn off all lights
    │   └── Floorplan
    ├── Settings ⏵
    │   ├── Brightness [50.0]
    │   ├── Screen timeout [2.0]
    │   ├── Rumbole [ON]
    │   ├── Sound [ON]
    │   └── Volume [2.0]
    ├── System ⏵
    │   ├── Status
    │   ├── WiFi [ON]
    │   ├── Restart
    │   └── Factory reset
    └── Close


### Games

#### Breakout
![Breakout Game Screenshot](assets/breakout-screen-1.png)

This is my take on Breakout / Arkanoid. It's played by moving the paddle with the rotary encoder. Features:

- Level progression: more bricks, power-ups and speed on each level
- Power-ups:
    - Extra life
    - Widen paddle
    - Wonky bricks: moves bricks around randomly
    - Multiball: spawn a new ball at the top of the screen above the paddle (max 10 balls simultaneously)
    - Shield which saves one ball from missing the paddle, can be stacked
    - Shooter: gives the paddle a shooter which shoots projectiles at the bricks damaging them, can be stacked once for a dual shooter
    - Question mark: shuffle all powerups randomly
- Autoplay: the game can play itself


[Code](clockos/packages/games/breakout/breakout.cpp)

#### Snake
Simple Snake clone, features:
- Control snake direction by turning the head left & right with the rotary encoder
- 4 pickups available at all times
- Ability to change game speed
- Ability to enable/disable walls
    - When disabled the snake warps to the other side when crossing the screen edge

[Code](clockos/packages/games/snake/snake.cpp)

# Credits
- First of all, shoutout to ESPHome for making this possible!
- [Peter Akimoto](https://x.com/pakimotochi)
    - Artistic supervision & motivation ❤️
    - Tokyo Bunny
    - Bunny Wiggle animation
