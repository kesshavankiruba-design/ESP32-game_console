<img width="500" height="601" alt="Screenshot 2026-10-17 153455" src="https://github.com/user-attachments/assets/d7620979-2b1b-4629-a06e-0d901aae8c31" />
# ESP32 Mini Arcade Gaming System

A mini arcade gaming system built using an **ESP32 microcontroller**, a **0.96-inch OLED display**, a **joystick**, and a **push button**. The ESP32 processes player inputs, runs the game logic, and displays graphics, scores, and menus on the OLED screen.

The system features four classic arcade-style games: Tetris, Space Shooter, Dino Run, and Pac-Man. This project combines embedded programming, electronics, and game development into a compact and interactive gaming experience.

## Hardware Components

- ESP32-DevKitC-32D microcontroller
- 0.96-inch OLED display (I²C)
- Joystick module
- Tactile push button
- 330 Ω resistor
- Jumper wires and circuit connections

## Circuit Design

### Circuit Schematic

The schematic shows the planned electrical connections between the ESP32, OLED display, joystick, push button, and resistor.
<img width="835" height="702" alt="Screenshot 2026-10-17 153448" src="https://github.com/user-attachments/assets/bf32a129-9dcf-41d3-8526-f5c062b062bd" />


### PCB Layout

The PCB layout illustrates the component placement and wiring routes for the mini arcade gaming system.
<img width="500" height="601" alt="Screenshot 2026-10-17 153455" src="https://github.com/user-attachments/assets/919f0d92-e566-4867-b352-fd4136ca6bc3" />


## Games

### 1. Tetris
Arrange falling blocks to complete rows and earn points. The goal is to clear as many rows as possible without filling the screen.

### 2. Space Shooter
Control a spaceship, avoid incoming enemies, and shoot targets to achieve a high score.

### 3. Dino Run
Jump over obstacles and survive for as long as possible while the game gets progressively more challenging.

### 4. Pac-Man
Navigate a maze, collect dots, and avoid ghosts while trying to achieve the highest score.

## Controls

- **Joystick:** Move the player, navigate menus, and control gameplay.
- **Push Button:** Select games and interact with menu options.
- **OLED Display:** Shows menus, gameplay, graphics, and scores.

## Software and Libraries

- **Programming Language:** Arduino C++
- **Development Environment:** Arduino IDE
- **Graphics Library:** Adafruit GFX
- **OLED Display Library:** Adafruit SSD1306

## How It Works

1. The ESP32 initializes the OLED display and reads input from the joystick and push button.
2. The player navigates the game menu using the joystick.
3. The push button selects a game or confirms a menu option.
4. The ESP32 executes the selected game's logic.
5. The OLED updates the screen with game graphics, player movements, and scores.

## Setup

1. Connect the ESP32, OLED display, joystick, and push button according to the circuit schematic.
2. Install the Arduino IDE.
3. Install the required libraries, including Adafruit GFX and Adafruit SSD1306.
4. Open the project source code in the Arduino IDE.
5. Select the correct ESP32 board and serial port.
6. Upload the code to the ESP32.
7. Power on the system and use the joystick and push button to navigate the games.

## Project Goals

- Develop interactive games using an ESP32 microcontroller.
- Learn embedded programming with Arduino C++.
- Integrate electronic components and input devices.
- Display graphics and game information on an OLED screen.
- Combine hardware design, software development, and game logic in one project.

## Project Status

The project is designed around four arcade games: Tetris, Space Shooter, Dino Run, and Pac-Man. Hardware connections, individual games, and controls should be tested as development progresses.

## Future Improvements

- Add more arcade games.
- Improve graphics and animations.
- Add sound effects.
- Implement high-score storage.
- Optimize controls and gameplay performance.

---

**Built with:** ESP32 · Arduino C++ · OLED Display · Joystick · Adafruit GFX · Adafruit SSD1306
