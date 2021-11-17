# G-Power

A custom PACMAN application using QT, using a gesture sensor to control the movements of the player character with hand motions.

### Implemented or Desired Features
- [x] Automatically exit game on win or loss
- [x] Freeze ghosts when cherry is eaten
- [ ] Multiple levels with increased difficulty
- [ ] Change 'theme' of ghosts and player character

## Prerequisites

### Hardware Required:
  - 1x  Beaglebone Black running a linux distro
  - 1x APDS-9960 RGB and Gesture Sensor
  - 1x Pushbutton Switch
  - 1x 330Ω Resistor
  - 1x Breadboard (Optional)

  For hardware setup, please refer to the wiring diagram: "G-Power/APDS-9960 to BBB Wiring Diagram.png"
  ![Wiring Diagram](https://github.com/sfagin89/G-Power/blob/main/APDS-9960%20to%20BBB%20Wiring%20Diagram.png?raw=true)

### Software Required:
  - Ability to cross-compile for ARM architecture (Unless compiling directly on the Beaglebone)
  - Ability to compile QT code

## Steps to Run from a Beaglebone Black:

1. Download and unzip the G-Power repo

2. From inside the APDS9960_BeagleBone directory, run `make` to produce the gesture controller binary.

3. From inside the pacman-milestone directory, run `qmake` then `make` to produce the QT binary.

  **IMPORTANT:** If you're going to run the game as a user other than root, open the mainwindow.cpp file and change the file paths at line 18 & 96 to your home directory prior to running the qmake and make commands.

You should have two binaries now, APDS9960_i2c_BBB & Gpower

4. Move both binaries to your home directory on the BeagleBone Black

**IMPORTANT:** You may need to change the permission level of the binary files using the `chmod` command before you can execute them.

5. Run the Gpower binary in the background using the command: `./Gpower &`

6. Run the APDS9960_i2c_BBB binary using the command `./APDS9960_i2c_BBB`

## How to Play:

Wave your hand over the gesture sensor in the direction you want PACMAN to move. PACMAN will move in that direction until you gesture in a different direction or until hitting a wall.

Collect coins through the maze to increase your score. Collect all the coins to win. Avoid the ghosts or lose the game. Eating a fruit will freeze the ghosts in place for a short period of time.

Game will continue until you win or lose.

When done with the game, disconnect the gesture 'controller' by pressing the Pushbutton.

Most Important: Have Fun!

## Resources Used:

1. Company, The Qt. “Cross-Platform Software Development for Embedded & Desktop.” Qt, www.qt.io/

2. Sparkfun. “APDS-9960 Datasheet ”. https://cdn.sparkfun.com/datasheets/Sensors/Proximity/apds9960.pdf

3. Github. “APDS-9960_RGB_and_Gesture_Sensor” C++ Library for Arduino, https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor.git

4. Blueskyson. “Blueskyson/Qt-Pac-Man.” GitHub, github.com/blueskyson/Qt-pac-man

5. VoidRealms. “C++ Qt 81 - QGraphicsView and QGraphicsScene.” YouTube, YouTube, 26 May 2011, www.youtube.com/watch?v=b35JF4LqtBs

6. hGenhGen 1, et al. “Invalid New-Expression of Abstract Class Type.” Stack Overflow, 1 Feb. 1963, stackoverflow.com/questions/23827014/invalid-new-expression-of-abstract-class-type/23827093
