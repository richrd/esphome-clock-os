# ESPHome ClockOS

ESPHome ClockOS is my work in progress project for an ESPHome based desk clock with an OLED screen and rotary encoder.


## Menu system
The default screen is the Clock, and clicking the encoder opens the main menu, below is the menu hierarchy which outlines the current features of the project:

    [ Main Menu ]
    ├── Clock
    ├── Games ⏵
    │   ├── Breakout ⏵
    │   │   ├── Play
    │   │   └── Autoplay [OFF]
    │   ├── Snake ⏵
    │   │   ├── Play
    │   │   ├── Speed [7.0]
    │   │   └── Walls [ON]
    │   └── Demos ⏵
    │       ├── Spin (spinning ball animation)
    │       └── DVD (Bouncing DVD logo screensaver)
    ├── Sounds ⏵
    │   ├── Mario
    │   ├── Star Wars
    │   ├── Mission Impossible
    │   └── Barbie Girl
    ├── Settings ⏵
    │   ├── Brightness [50.0]
    │   ├── Rumbole [ON]
    │   ├── Sound [ON]
    │   ├── Volume [2.0]
    │   ├── System ⏵
    │   │   └── [Shows system information screen]
    │   └── Restart
    │       └── [Restarts the ESP32]
    └── Close



## Games

### Breakout
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
- Autoplay: the game can play itself


[Code](games/breakout.cpp)

### Snake
Simple Snake clone, features:
- Control snake direction by turning the head left & right with the rotary encoder
- 4 pickups available at all times
- Ability to change game speed
- Ability to enable/disable walls (snake warps to the other side when crossing a wall)

[Code](games/snake.cpp)

