#include <Wire.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED DISPLAY
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define OLED_SDA 22
#define OLED_SCL 23

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// JOYSTICK
// =====================================================

const int JOY_X = 2;
const int JOY_Y = 4;

const int JOYSTICK_BUTTON = 21;
const int FIRE_BUTTON = 5;

const int DEADZONE_LOW = 1400;
const int DEADZONE_HIGH = 2700;

// =====================================================
// GAME MODES
// =====================================================

enum GameMode {
  MENU,
  SPACE_GAME,
  DINO_GAME,
  TETRIS_GAME,
  PACMAN_GAME
};

GameMode currentMode = MENU;

int selectedGame = 0;  // 0 = Space, 1 = Dinosaur, 2 = Tetris, 3 = Pac-Man

bool previousJoystickButton = HIGH;
bool previousFireButton = HIGH;
bool wasJoystickRight = false;
bool wasJoystickLeft = false;

// =====================================================
// TIMING
// =====================================================

unsigned long lastFrame = 0;
const int FRAME_DELAY = 35;

// =====================================================
// SPACE GAME VARIABLES
// =====================================================

const int SHIP_W = 9;
const int SHIP_H = 10;

const int MAX_ROCKS = 6;
const int MAX_BULLETS = 4;

struct Rock {
  int x;
  int y;
  int size;
  bool active;
};

struct Bullet {
  int x;
  int y;
  bool active;
};

Rock rocks[MAX_ROCKS];
Bullet bullets[MAX_BULLETS];

int shipX = 64;
int shipY = 48;
int spaceScore = 0;

bool spaceGameOver = false;

unsigned long lastRock = 0;
unsigned long lastShot = 0;

// =====================================================
// DINOSAUR GAME VARIABLES
// =====================================================

const int groundY = 53;

const int dinoX = 15;
const int dinoW = 9;
const int dinoH = 10;

int dinoY = groundY - dinoH;
int velocityY = 0;

const int gravity = 1;
const int jumpVelocity = -7;

int cactusX = SCREEN_WIDTH;
const int cactusW = 6;
const int cactusH = 12;

int dinoScore = 0;
int gameSpeed = 3;

bool dinoGameOver = false;

// =====================================================
// DRAW AIRSHIP
// =====================================================

void drawShipAt(int x, int y) {
  display.fillTriangle(
    x, y - SHIP_H / 2,
    x - SHIP_W / 2, y + SHIP_H / 2,
    x + SHIP_W / 2, y + SHIP_H / 2,
    SSD1306_WHITE
  );

  display.drawPixel(x, y, SSD1306_BLACK);

  display.drawLine(
    x - 3, y + 2,
    x - 6, y + 4,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 3, y + 2,
    x + 6, y + 4,
    SSD1306_WHITE
  );
}

void drawShip() {
  drawShipAt(shipX, shipY);
}

// =====================================================
// DRAW DINOSAUR
// =====================================================

void drawDino(int x, int y, bool running) {
  display.fillRect(
    x + 1, y + 3, 6, 6,
    SSD1306_WHITE
  );

  display.fillRect(
    x + 3, y, 6, 5,
    SSD1306_WHITE
  );

  display.drawPixel(
    x + 7, y + 1,
    SSD1306_BLACK
  );

  display.drawPixel(
    x, y + 5,
    SSD1306_WHITE
  );

  if (running) {
    display.drawLine(
      x + 2, y + 8,
      x + 1, y + 9,
      SSD1306_WHITE
    );

    display.drawLine(
      x + 5, y + 8,
      x + 6, y + 9,
      SSD1306_WHITE
    );
  } else {
    display.drawLine(
      x + 2, y + 8,
      x + 2, y + 9,
      SSD1306_WHITE
    );

    display.drawLine(
      x + 5, y + 8,
      x + 5, y + 9,
      SSD1306_WHITE
    );
  }
}

// =====================================================
// DRAW CACTUS
// =====================================================

void drawCactus(int x) {
  display.fillRect(
    x + 2,
    groundY - cactusH,
    3,
    cactusH,
    SSD1306_WHITE
  );

  display.drawLine(
    x, groundY - 7,
    x, groundY - 3,
    SSD1306_WHITE
  );

  display.drawPixel(
    x + 1, groundY - 7,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 5, groundY - 9,
    x + 5, groundY - 5,
    SSD1306_WHITE
  );

  display.drawPixel(
    x + 4, groundY - 9,
    SSD1306_WHITE
  );
}

// =====================================================
// MENU AIRCRAFT ICON
// =====================================================

void drawMenuAircraft() {
  display.fillTriangle(
    64, 17,
    57, 31,
    71, 31,
    SSD1306_WHITE
  );

  display.fillTriangle(
    64, 23,
    49, 31,
    57, 32,
    SSD1306_WHITE
  );

  display.fillTriangle(
    64, 23,
    71, 32,
    79, 31,
    SSD1306_WHITE
  );

  display.drawLine(
    61, 30, 61, 34,
    SSD1306_WHITE
  );

  display.drawLine(
    67, 30, 67, 34,
    SSD1306_WHITE
  );
}

// =====================================================
// MENU DINOSAUR ICON
// =====================================================

void drawMenuDinosaur() {
  display.fillRect(
    57, 26, 17, 10,
    SSD1306_WHITE
  );

  display.fillRect(
    65, 19, 12, 10,
    SSD1306_WHITE
  );

  display.fillRect(
    73, 24, 6, 5,
    SSD1306_WHITE
  );

  display.drawPixel(
    73, 21,
    SSD1306_BLACK
  );

  display.fillTriangle(
    58, 28,
    49, 23,
    51, 32,
    SSD1306_WHITE
  );

  display.drawLine(
    60, 34, 60, 39,
    SSD1306_WHITE
  );

  display.drawLine(
    70, 34, 70, 39,
    SSD1306_WHITE
  );
}

void drawMenuTetris();
void drawMenuPacman();

// =====================================================
// DRAW HOME MENU
// =====================================================

void drawMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 1);
  display.print("SELECT A GAME");

  display.drawLine(
    0, 11, 127, 11,
    SSD1306_WHITE
  );

  if (selectedGame == 0) {
    drawMenuAircraft();
    display.setCursor(35, 39);
    display.print("> SPACE GAME");
  } else if (selectedGame == 1) {
    drawMenuDinosaur();
    display.setCursor(37, 39);
    display.print("> DINO GAME");
  } else if (selectedGame == 2) {
    drawMenuTetris();
    display.setCursor(43, 39);
    display.print("> TETRIS");
  } else {
    drawMenuPacman();
    display.setCursor(43, 39);
    display.print("> PAC-MAN");
  }

  display.setCursor(13, 53);
  display.print("L/R CHANGE  SW PLAY");

  display.display();
}

// =====================================================
// RESET SPACE GAME
// =====================================================

void resetSpaceGame() {
  shipX = 64;
  shipY = 48;
  spaceScore = 0;
  spaceGameOver = false;

  for (int i = 0; i < MAX_ROCKS; i++) {
    rocks[i].active = false;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }

  lastRock = millis();
  lastShot = 0;
}

// =====================================================
// SPAWN ROCK
// =====================================================

void spawnRock() {
  for (int i = 0; i < MAX_ROCKS; i++) {
    if (!rocks[i].active) {
      rocks[i].x = random(5, SCREEN_WIDTH - 5);
      rocks[i].y = -5;
      rocks[i].size = random(3, 6);
      rocks[i].active = true;
      return;
    }
  }
}

// =====================================================
// DRAW ROCKS
// =====================================================

void drawRocks() {
  for (int i = 0; i < MAX_ROCKS; i++) {
    if (!rocks[i].active) {
      continue;
    }

    int x = rocks[i].x;
    int y = rocks[i].y;
    int s = rocks[i].size;

    display.drawCircle(
      x, y, s,
      SSD1306_WHITE
    );

    display.drawPixel(
      x - 1, y - 1,
      SSD1306_WHITE
    );

    display.drawPixel(
      x + 2, y + 1,
      SSD1306_WHITE
    );
  }
}

// =====================================================
// SHOOT BULLET
// =====================================================

void shoot() {
  if (millis() - lastShot < 180) {
    return;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      bullets[i].x = shipX;
      bullets[i].y = shipY - SHIP_H / 2;
      bullets[i].active = true;

      lastShot = millis();
      return;
    }
  }
}

// =====================================================
// DRAW BULLETS
// =====================================================

void drawBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      display.drawLine(
        bullets[i].x,
        bullets[i].y,
        bullets[i].x,
        bullets[i].y - 3,
        SSD1306_WHITE
      );
    }
  }
}

// =====================================================
// UPDATE SPACE GAME
// =====================================================

void updateSpaceGame() {
  int joyX = analogRead(JOY_X);
  int joyY = analogRead(JOY_Y);

  if (joyX < DEADZONE_LOW) {
    shipX -= 2;
  } else if (joyX > DEADZONE_HIGH) {
    shipX += 2;
  }

  if (joyY < DEADZONE_LOW) {
    shipY -= 2;
  } else if (joyY > DEADZONE_HIGH) {
    shipY += 2;
  }

  shipX = constrain(shipX, 7, SCREEN_WIDTH - 7);
  shipY = constrain(shipY, 17, SCREEN_HEIGHT - 6);

  int spawnDelay = max(250, 850 - spaceScore * 12);

  if (millis() - lastRock >=
      (unsigned long)spawnDelay) {
    spawnRock();
    lastRock = millis();
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      continue;
    }

    bullets[i].y -= 4;

    if (bullets[i].y < 0) {
      bullets[i].active = false;
    }
  }

  int rockSpeed = min(4, 1 + spaceScore / 10);

  for (int i = 0; i < MAX_ROCKS; i++) {
    if (!rocks[i].active) {
      continue;
    }

    rocks[i].y += rockSpeed;

    if (rocks[i].y >
        SCREEN_HEIGHT + rocks[i].size) {
      rocks[i].active = false;
      continue;
    }

    // Ship-rock collision
    int dx = abs(rocks[i].x - shipX);
    int dy = abs(rocks[i].y - shipY);

    if (dx < rocks[i].size + 4 &&
        dy < rocks[i].size + 4) {
      spaceGameOver = true;
      return;
    }

    // Bullet-rock collision
    for (int j = 0; j < MAX_BULLETS; j++) {
      if (!bullets[j].active) {
        continue;
      }

      int bx = abs(bullets[j].x - rocks[i].x);
      int by = abs(bullets[j].y - rocks[i].y);

      if (bx <= rocks[i].size &&
          by <= rocks[i].size + 2) {
        bullets[j].active = false;
        rocks[i].active = false;
        spaceScore++;
        break;
      }
    }
  }
}

// =====================================================
// DRAW SPACE GAME
// =====================================================

void drawSpaceGame() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("SCORE: ");
  display.print(spaceScore);

  display.drawLine(
    0, 10, 127, 10,
    SSD1306_WHITE
  );

  drawRocks();
  drawBullets();
  drawShip();

  display.display();
}

// =====================================================
// SPACE GAME OVER
// =====================================================

void drawSpaceGameOver() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);

  display.setCursor(10, 8);
  display.println("GAME OVER");

  display.setTextSize(1);
  display.setCursor(38, 34);
  display.print("Score: ");
  display.println(spaceScore);

  display.setCursor(4, 45);
  display.print("GPIO 5: Play again");

  display.setCursor(4, 55);
  display.print("SW: Back to menu");

  display.display();
}

// =====================================================
// RESET DINOSAUR GAME
// =====================================================

void resetDinoGame() {
  dinoY = groundY - dinoH;
  velocityY = 0;

  cactusX = SCREEN_WIDTH;
  dinoScore = 0;
  gameSpeed = 3;

  dinoGameOver = false;
}

// =====================================================
// UPDATE DINOSAUR GAME
// =====================================================

void updateDinoGame() {
  dinoY += velocityY;
  velocityY += gravity;

  if (dinoY >= groundY - dinoH) {
    dinoY = groundY - dinoH;
    velocityY = 0;
  }

  cactusX -= gameSpeed;

  if (cactusX + cactusW < dinoX) {
    dinoScore++;

    cactusX = SCREEN_WIDTH + random(0, 26);

    if (dinoScore % 5 == 0 && gameSpeed < 7) {
      gameSpeed++;
    }
  }

  bool hitX =
    dinoX + dinoW - 1 >= cactusX &&
    dinoX + 2 <= cactusX + cactusW;

  bool hitY =
    dinoY + dinoH >= groundY - cactusH &&
    dinoY < groundY;

  if (hitX && hitY) {
    dinoGameOver = true;
  }
}

// =====================================================
// DRAW DINOSAUR GAME
// =====================================================

void drawDinoGame() {
  display.clearDisplay();

  display.drawLine(
    0, groundY, 127, groundY,
    SSD1306_WHITE
  );

  drawDino(
    dinoX,
    dinoY,
    dinoY == groundY - dinoH
  );

  drawCactus(cactusX);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(90, 2);
  display.print("S:");
  display.print(dinoScore);

  display.display();
}

// =====================================================
// DINOSAUR GAME OVER
// =====================================================

void drawDinoGameOver() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(28, 8);
  display.print("GAME OVER");

  display.setCursor(37, 22);
  display.print("Score: ");
  display.print(dinoScore);

  display.setCursor(4, 40);
  display.print("GPIO 5: Play again");

  display.setCursor(4, 53);
  display.print("SW: Back to menu");

  display.display();
}


// =====================================================
// TETRIS GAME VARIABLES
// =====================================================

const int TETRIS_COLS = 10;
const int TETRIS_ROWS = 13;
const int TETRIS_CELL = 4;
const int TETRIS_X = 5;
const int TETRIS_Y = 11;

uint8_t tetrisBoard[TETRIS_ROWS][TETRIS_COLS];

// Each piece is stored as four 4-bit rows.
const uint8_t tetrisShapes[7][4] = {
  {0b0000, 0b1111, 0b0000, 0b0000}, // I
  {0b0110, 0b0110, 0b0000, 0b0000}, // O
  {0b0100, 0b1110, 0b0000, 0b0000}, // T
  {0b0110, 0b1100, 0b0000, 0b0000}, // S
  {0b1100, 0b0110, 0b0000, 0b0000}, // Z
  {0b1000, 0b1110, 0b0000, 0b0000}, // J
  {0b0010, 0b1110, 0b0000, 0b0000}  // L
};

int tetrisPiece = 0;
int tetrisRotation = 0;
int tetrisPieceX = 3;
int tetrisPieceY = 0;
int tetrisScore = 0;
bool tetrisGameOver = false;
unsigned long lastTetrisFall = 0;
unsigned long lastTetrisMove = 0;

// =====================================================
// PAC-MAN GAME VARIABLES
// =====================================================

const int PAC_COLS = 21;
const int PAC_ROWS = 11;
const int PAC_CELL = 5;
const int PAC_X = 11;
const int PAC_Y = 9;

const char pacMazeTemplate[PAC_ROWS][PAC_COLS + 1] = {
  "#####################",
  "#.........#.........#",
  "#.###.###.#.###.###.#",
  "#o#...#...#...#...#o#",
  "#.###.#.#####.#.###.#",
  "#.....#...#...#.....#",
  "#####.###...###.#####",
  "#......... .........#",
  "#.###.###.#.###.###.#",
  "#.........#.........#",
  "#####################"
};

char pacMaze[PAC_ROWS][PAC_COLS + 1];
int pacX = 1;
int pacY = 1;
int pacScore = 0;
int pacLives = 3;
int pacPelletsLeft = 0;
bool pacGameOver = false;
unsigned long lastPacMove = 0;
unsigned long lastGhostMove = 0;

const int PAC_GHOSTS = 2;
int ghostX[PAC_GHOSTS] = {19, 10};
int ghostY[PAC_GHOSTS] = {1, 9};

// =====================================================
// DRAW TETRIS MENU ICON
// =====================================================

void drawMenuTetris() {
  display.drawRect(48, 16, 32, 22, SSD1306_WHITE);
  display.fillRect(52, 20, 6, 6, SSD1306_WHITE);
  display.fillRect(58, 20, 6, 6, SSD1306_WHITE);
  display.fillRect(64, 20, 6, 6, SSD1306_WHITE);
  display.fillRect(58, 26, 6, 6, SSD1306_WHITE);
  display.fillRect(64, 26, 6, 6, SSD1306_WHITE);
  display.fillRect(70, 26, 6, 6, SSD1306_WHITE);
}

// =====================================================
// DRAW PAC-MAN MENU ICON
// =====================================================

void drawMenuPacman() {
  display.fillCircle(64, 27, 12, SSD1306_WHITE);
  display.fillTriangle(
    64, 27, 78, 20, 78, 34,
    SSD1306_BLACK
  );
  display.drawPixel(61, 20, SSD1306_BLACK);
}

// =====================================================
// TETRIS HELPERS
// =====================================================

bool tetrisCell(int shape, int rotation, int row, int col) {
  int baseRow;
  int baseCol;

  if (rotation == 0) {
    baseRow = row;
    baseCol = col;
  } else if (rotation == 1) {
    baseRow = 3 - col;
    baseCol = row;
  } else if (rotation == 2) {
    baseRow = 3 - row;
    baseCol = 3 - col;
  } else {
    baseRow = col;
    baseCol = 3 - row;
  }

  if (baseRow < 0 || baseRow > 3 ||
      baseCol < 0 || baseCol > 3) {
    return false;
  }

  return (tetrisShapes[shape][baseRow] &
          (1 << (3 - baseCol))) != 0;
}

bool tetrisCanPlace(int shape, int rotation, int px, int py) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (!tetrisCell(shape, rotation, row, col)) {
        continue;
      }

      int bx = px + col;
      int by = py + row;

      if (bx < 0 || bx >= TETRIS_COLS ||
          by >= TETRIS_ROWS) {
        return false;
      }

      if (by >= 0 && tetrisBoard[by][bx]) {
        return false;
      }
    }
  }

  return true;
}

void spawnTetrisPiece() {
  tetrisPiece = random(0, 7);
  tetrisRotation = 0;
  tetrisPieceX = 3;
  tetrisPieceY = 0;

  if (!tetrisCanPlace(
        tetrisPiece, tetrisRotation,
        tetrisPieceX, tetrisPieceY)) {
    tetrisGameOver = true;
  }
}

void clearTetrisLines() {
  for (int row = TETRIS_ROWS - 1; row >= 0; row--) {
    bool full = true;

    for (int col = 0; col < TETRIS_COLS; col++) {
      if (!tetrisBoard[row][col]) {
        full = false;
        break;
      }
    }

    if (full) {
      for (int moveRow = row; moveRow > 0; moveRow--) {
        for (int col = 0; col < TETRIS_COLS; col++) {
          tetrisBoard[moveRow][col] =
            tetrisBoard[moveRow - 1][col];
        }
      }

      for (int col = 0; col < TETRIS_COLS; col++) {
        tetrisBoard[0][col] = 0;
      }

      tetrisScore += 10;
      row++; // Check this row again after shifting.
    }
  }
}

void lockTetrisPiece() {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (!tetrisCell(
            tetrisPiece, tetrisRotation, row, col)) {
        continue;
      }

      int bx = tetrisPieceX + col;
      int by = tetrisPieceY + row;

      if (by >= 0 && by < TETRIS_ROWS &&
          bx >= 0 && bx < TETRIS_COLS) {
        tetrisBoard[by][bx] = 1;
      }
    }
  }

  clearTetrisLines();
  spawnTetrisPiece();
}

void resetTetrisGame() {
  memset(tetrisBoard, 0, sizeof(tetrisBoard));
  tetrisScore = 0;
  tetrisGameOver = false;
  lastTetrisFall = millis();
  lastTetrisMove = 0;
  spawnTetrisPiece();
}

void updateTetrisGame(bool firePressed) {
  int joyX = analogRead(JOY_X);
  int joyY = analogRead(JOY_Y);
  unsigned long now = millis();

  if (now - lastTetrisMove >= 135) {
    if (joyX < DEADZONE_LOW &&
        tetrisCanPlace(tetrisPiece, tetrisRotation,
                       tetrisPieceX - 1, tetrisPieceY)) {
      tetrisPieceX--;
      lastTetrisMove = now;
    } else if (joyX > DEADZONE_HIGH &&
               tetrisCanPlace(tetrisPiece, tetrisRotation,
                              tetrisPieceX + 1, tetrisPieceY)) {
      tetrisPieceX++;
      lastTetrisMove = now;
    } else if (joyY > DEADZONE_HIGH &&
               tetrisCanPlace(tetrisPiece, tetrisRotation,
                              tetrisPieceX, tetrisPieceY + 1)) {
      tetrisPieceY++;
      lastTetrisMove = now;
    }
  }

  if (firePressed) {
    int nextRotation = (tetrisRotation + 1) % 4;
    if (tetrisCanPlace(tetrisPiece, nextRotation,
                       tetrisPieceX, tetrisPieceY)) {
      tetrisRotation = nextRotation;
    }
  }

  int fallDelay = max(100, 480 - tetrisScore * 2);
  if (now - lastTetrisFall >= (unsigned long)fallDelay) {
    lastTetrisFall = now;

    if (tetrisCanPlace(tetrisPiece, tetrisRotation,
                       tetrisPieceX, tetrisPieceY + 1)) {
      tetrisPieceY++;
    } else {
      lockTetrisPiece();
    }
  }
}

void drawTetrisGame() {
  display.clearDisplay();

  display.drawRect(
    TETRIS_X - 1, TETRIS_Y - 1,
    TETRIS_COLS * TETRIS_CELL + 2,
    TETRIS_ROWS * TETRIS_CELL + 2,
    SSD1306_WHITE
  );

  for (int row = 0; row < TETRIS_ROWS; row++) {
    for (int col = 0; col < TETRIS_COLS; col++) {
      if (tetrisBoard[row][col]) {
        display.fillRect(
          TETRIS_X + col * TETRIS_CELL,
          TETRIS_Y + row * TETRIS_CELL,
          TETRIS_CELL - 1, TETRIS_CELL - 1,
          SSD1306_WHITE
        );
      }
    }
  }

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (tetrisCell(
            tetrisPiece, tetrisRotation, row, col)) {
        int bx = tetrisPieceX + col;
        int by = tetrisPieceY + row;

        if (by >= 0 && by < TETRIS_ROWS) {
          display.fillRect(
            TETRIS_X + bx * TETRIS_CELL,
            TETRIS_Y + by * TETRIS_CELL,
            TETRIS_CELL - 1, TETRIS_CELL - 1,
            SSD1306_WHITE
          );
        }
      }
    }
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(53, 15);
  display.print("TETRIS");
  display.setCursor(53, 29);
  display.print("Score");
  display.setCursor(53, 39);
  display.print(tetrisScore);
  display.setCursor(53, 51);
  display.print("SW: Menu");

  display.display();
}

void drawTetrisGameOver() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(28, 8);
  display.print("TETRIS OVER");
  display.setCursor(34, 22);
  display.print("Score: ");
  display.print(tetrisScore);
  display.setCursor(4, 40);
  display.print("GPIO 5: Play again");
  display.setCursor(4, 53);
  display.print("SW: Back to menu");
  display.display();
}

// =====================================================
// PAC-MAN HELPERS
// =====================================================

bool pacWalkable(int x, int y) {
  if (x < 0 || x >= PAC_COLS ||
      y < 0 || y >= PAC_ROWS) {
    return false;
  }

  return pacMaze[y][x] != '#';
}

void resetPacmanGame() {
  pacScore = 0;
  pacLives = 3;
  pacGameOver = false;
  pacX = 1;
  pacY = 1;
  ghostX[0] = 19;
  ghostY[0] = 1;
  ghostX[1] = 10;
  ghostY[1] = 9;
  pacPelletsLeft = 0;

  for (int row = 0; row < PAC_ROWS; row++) {
    for (int col = 0; col < PAC_COLS; col++) {
      pacMaze[row][col] = pacMazeTemplate[row][col];
      if (pacMaze[row][col] == '.' ||
          pacMaze[row][col] == 'o') {
        pacPelletsLeft++;
      }
    }
    pacMaze[row][PAC_COLS] = '\0';
  }

  lastPacMove = millis();
  lastGhostMove = millis();
}

void movePacman() {
  int joyX = analogRead(JOY_X);
  int joyY = analogRead(JOY_Y);
  int nextX = pacX;
  int nextY = pacY;

  // One joystick direction is read each movement step.
  if (joyX < DEADZONE_LOW) {
    nextX--;
  } else if (joyX > DEADZONE_HIGH) {
    nextX++;
  } else if (joyY < DEADZONE_LOW) {
    nextY--;
  } else if (joyY > DEADZONE_HIGH) {
    nextY++;
  }

  if (pacWalkable(nextX, nextY)) {
    pacX = nextX;
    pacY = nextY;

    if (pacMaze[pacY][pacX] == '.' ||
        pacMaze[pacY][pacX] == 'o') {
      pacScore += (pacMaze[pacY][pacX] == 'o') ? 5 : 1;
      pacMaze[pacY][pacX] = ' ';
      pacPelletsLeft--;
    }
  }
}

void moveGhosts() {
  for (int i = 0; i < PAC_GHOSTS; i++) {
    int direction = random(0, 4);
    int nx = ghostX[i];
    int ny = ghostY[i];

    if (direction == 0) nx--;
    else if (direction == 1) nx++;
    else if (direction == 2) ny--;
    else ny++;

    if (pacWalkable(nx, ny)) {
      ghostX[i] = nx;
      ghostY[i] = ny;
    }
  }
}

void checkPacmanCollision() {
  for (int i = 0; i < PAC_GHOSTS; i++) {
    if (pacX == ghostX[i] && pacY == ghostY[i]) {
      pacLives--;

      if (pacLives <= 0) {
        pacGameOver = true;
      } else {
        pacX = 1;
        pacY = 1;
        ghostX[0] = 19;
        ghostY[0] = 1;
        ghostX[1] = 10;
        ghostY[1] = 9;
      }

      return;
    }
  }

  if (pacPelletsLeft <= 0) {
    pacGameOver = true;
  }
}

void updatePacmanGame() {
  unsigned long now = millis();

  if (now - lastPacMove >= 135) {
    lastPacMove = now;
    movePacman();
  }

  if (now - lastGhostMove >= 420) {
    lastGhostMove = now;
    moveGhosts();
  }

  checkPacmanCollision();
}

void drawPacmanGame() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("S:");
  display.print(pacScore);
  display.setCursor(78, 0);
  display.print("L:");
  display.print(pacLives);

  for (int row = 0; row < PAC_ROWS; row++) {
    for (int col = 0; col < PAC_COLS; col++) {
      int px = PAC_X + col * PAC_CELL;
      int py = PAC_Y + row * PAC_CELL;
      char tile = pacMaze[row][col];

      if (tile == '#') {
        display.drawRect(
          px, py, PAC_CELL, PAC_CELL,
          SSD1306_WHITE
        );
      } else if (tile == '.') {
        display.drawPixel(
          px + PAC_CELL / 2,
          py + PAC_CELL / 2,
          SSD1306_WHITE
        );
      } else if (tile == 'o') {
        display.drawCircle(
          px + PAC_CELL / 2,
          py + PAC_CELL / 2, 1,
          SSD1306_WHITE
        );
      }
    }
  }

  // Draw the two ghosts.
  for (int i = 0; i < PAC_GHOSTS; i++) {
    display.fillRect(
      PAC_X + ghostX[i] * PAC_CELL + 1,
      PAC_Y + ghostY[i] * PAC_CELL + 1,
      PAC_CELL - 2, PAC_CELL - 2,
      SSD1306_WHITE
    );
  }

  // Draw Pac-Man as a small circle with a mouth.
  int playerPx = PAC_X + pacX * PAC_CELL + PAC_CELL / 2;
  int playerPy = PAC_Y + pacY * PAC_CELL + PAC_CELL / 2;
  display.fillCircle(playerPx, playerPy, 2, SSD1306_WHITE);
  display.drawPixel(playerPx + 2, playerPy, SSD1306_BLACK);

  display.display();
}

void drawPacmanGameOver() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(25, 8);
  display.print("PAC-MAN OVER");
  display.setCursor(35, 22);
  display.print("Score: ");
  display.print(pacScore);
  display.setCursor(4, 40);
  display.print("GPIO 5: Play again");
  display.setCursor(4, 53);
  display.print("SW: Back to menu");
  display.display();
}


// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);

  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);
  pinMode(FIRE_BUTTON, INPUT_PULLUP);

  analogReadResolution(12);

  // OLED SDA = 22, SCL = 23
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");

    while (true) {
      delay(100);
    }
  }

  randomSeed(analogRead(35) + micros());

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(25, 20);
  display.println("WELCOME!");

  display.setCursor(10, 36);
  display.println("Choose your game");

  display.display();

  delay(1200);

  resetSpaceGame();
  resetDinoGame();
  resetTetrisGame();
  resetPacmanGame();

  currentMode = MENU;
  selectedGame = 0;

  drawMenu();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
  int joyX = analogRead(JOY_X);

  bool joystickButtonNow =
    digitalRead(JOYSTICK_BUTTON);

  bool fireButtonNow =
    digitalRead(FIRE_BUTTON);

  bool joystickPressed =
    previousJoystickButton == HIGH &&
    joystickButtonNow == LOW;

  bool firePressed =
    previousFireButton == HIGH &&
    fireButtonNow == LOW;

  previousJoystickButton = joystickButtonNow;
  previousFireButton = fireButtonNow;

  // Detect fresh joystick movements
  bool joystickRight = joyX > DEADZONE_HIGH;
  bool joystickLeft = joyX < DEADZONE_LOW;

  bool movedRight =
    joystickRight && !wasJoystickRight;

  bool movedLeft =
    joystickLeft && !wasJoystickLeft;

  wasJoystickRight = joystickRight;
  wasJoystickLeft = joystickLeft;

  // ===================================================
  // HOME MENU
  // ===================================================

  if (currentMode == MENU) {
    if (movedRight) {
      selectedGame = (selectedGame + 1) % 4;
      drawMenu();
    } else if (movedLeft) {
      selectedGame = (selectedGame + 3) % 4;
      drawMenu();
    }

    if (joystickPressed) {
      if (selectedGame == 0) {
        resetSpaceGame();
        currentMode = SPACE_GAME;
      } else if (selectedGame == 1) {
        resetDinoGame();
        currentMode = DINO_GAME;
      } else if (selectedGame == 2) {
        resetTetrisGame();
        currentMode = TETRIS_GAME;
      } else {
        resetPacmanGame();
        currentMode = PACMAN_GAME;
      }

      lastFrame = millis();
    }

    return;
  }

  // ===================================================
  // SPACE GAME
  // ===================================================

  if (currentMode == SPACE_GAME) {
    if (spaceGameOver) {
      drawSpaceGameOver();

      // GPIO 21 returns to the home menu.
      if (joystickPressed) {
        currentMode = MENU;
        selectedGame = 0;
        drawMenu();
      }
      // GPIO 5 restarts the Space Game.
      else if (firePressed) {
        resetSpaceGame();
        lastFrame = millis();
        drawSpaceGame();
      }

      return;
    }

    if (firePressed) {
      shoot();
    }

    if (millis() - lastFrame < FRAME_DELAY) {
      return;
    }

    lastFrame = millis();

    updateSpaceGame();

    if (spaceGameOver) {
      drawSpaceGameOver();
    } else {
      drawSpaceGame();
    }

    return;
  }

  // ===================================================
  // DINOSAUR GAME
  // ===================================================

  if (currentMode == DINO_GAME) {
    if (dinoGameOver) {
      drawDinoGameOver();

      // GPIO 21 returns to the home menu.
      if (joystickPressed) {
        currentMode = MENU;
        selectedGame = 0;
        drawMenu();
      }
      // GPIO 5 restarts the Dinosaur Game.
      else if (firePressed) {
        resetDinoGame();
        lastFrame = millis();
        drawDinoGame();
      }

      return;
    }

    // GPIO 5 makes the dinosaur jump.
    if (firePressed &&
        dinoY >= groundY - dinoH) {
      velocityY = jumpVelocity;
    }

    if (millis() - lastFrame < 45) {
      return;
    }

    lastFrame = millis();

    updateDinoGame();

    if (dinoGameOver) {
      drawDinoGameOver();
    } else {
      drawDinoGame();
    }

    return;
  }

  // ===================================================
  // TETRIS GAME
  // ===================================================

  if (currentMode == TETRIS_GAME) {
    if (tetrisGameOver) {
      drawTetrisGameOver();

      if (joystickPressed) {
        currentMode = MENU;
        selectedGame = 0;
        drawMenu();
      } else if (firePressed) {
        resetTetrisGame();
        lastFrame = millis();
        drawTetrisGame();
      }

      return;
    }

    if (millis() - lastFrame < 35) {
      return;
    }

    lastFrame = millis();
    updateTetrisGame(firePressed);

    if (tetrisGameOver) {
      drawTetrisGameOver();
    } else {
      drawTetrisGame();
    }

    return;
  }

  // ===================================================
  // PAC-MAN GAME
  // ===================================================

  if (currentMode == PACMAN_GAME) {
    if (pacGameOver) {
      drawPacmanGameOver();

      if (joystickPressed) {
        currentMode = MENU;
        selectedGame = 0;
        drawMenu();
      } else if (firePressed) {
        resetPacmanGame();
        lastFrame = millis();
        drawPacmanGame();
      }

      return;
    }

    if (millis() - lastFrame < 30) {
      return;
    }

    lastFrame = millis();
    updatePacmanGame();

    if (pacGameOver) {
      drawPacmanGameOver();
    } else {
      drawPacmanGame();
    }

    return;
  }
}