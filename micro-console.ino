//
// Библиотеки
//
#define PIN_CS 5
// Одна матрица (1х1), пин CS на D5
#include "src/libs/GyverMAX7219.h"
MAX7219< 1, 1, PIN_CS > matrix;

//
// Константы
//
#define DEBUG 0
#define PIN_BTN_LEFT 2    // Левая кнопка пин D2
#define PIN_BTN_RIGHT 3   // Правая кнопка пин D3 
#define PIN_BTN_MIDDLE 4  // Кнопка действия пин D4 
#define GAME_COUNT 4 - 1

//
// Матрица
//
#define WIDTH 8
#define HEIGHT 8
#define H_BORDER HEIGHT - 1
#define W_BORDER WIDTH - 1

//
// Класс кнопок
//
#include "src/classes/Button.h"

Button btn_left(PIN_BTN_LEFT);
Button btn_right(PIN_BTN_RIGHT);
Button btn_middle(PIN_BTN_MIDDLE);

//
// Загрузка игр
//
#include "src/classes/Game.h"
#include "src/classes/Arkanoid.h"
#include "src/classes/Shooter.h"
#include "src/classes/Jump.h"

// Указатель текущую игру
Game* game;

//
// Инициализация
//
void setup() {
  #ifdef DEBUG 
  Serial.begin(9600);
  #endif

  // Матрица
  matrix.begin();
  matrix.setBright(1); // яркость 0..15

  // loadGame(new Jump());
}

//
// Основной цикл
//
byte gameIndex = 0;
bool state_menu = true;

// Отрисовка битовой матрицы
// https://xantorohara.github.io/led-matrix-editor
void displayByteImage(uint64_t image, int flag = 1) {
  for (int y = 0; y < 8; y++) { 
    byte row = (image >> (y * 8)) & 0xFF;
    for (int x = 0; x < 8; x++) {
      bool pixel = (row >> x) & 0x01;
      if (pixel)
        matrix.dot(x, y, flag);
    }
  }
}

void transition_effect() {
  matrix.clear();
  for (byte y = 0; y < H_BORDER; y++) {
    matrix.update();
    delay(100);
    for (byte x = 0; x < WIDTH; x++) {
      matrix.dot(x, y);
      matrix.dot(x, H_BORDER - y);
    }
  }

  for (byte y = H_BORDER; y > 0; y--) {
    matrix.update();
    delay(100);
    for (byte x = 0; x < WIDTH; x++) {
      matrix.dot(x, y, 0);
      matrix.dot(x, H_BORDER - y, 0);
    }
  }
}

void loadGame(Game* newGame) {
  if (game) {
    delete game;
  }
  game = newGame;
  game->init();
  state_menu = false;
}

// Переменные для анимации
int imageIndex = 0;
unsigned long timeout_50ms = 0;

void loop() {
  keys_update();

  if (state_menu) {
    switch (gameIndex) {
      case 0:
        matrix.clear();
        displayByteImage(0x0038101010141800);
        matrix.update();

        if (btn_middle.getState() == PRESS) {
          transition_effect();
          loadGame(new Arkanoid());
        }
      break;
      case 1:
        matrix.clear();
        displayByteImage(0x003c081020243800);
        matrix.update();

        if (btn_middle.getState() == PRESS) {
          transition_effect();
          loadGame(new Shooter());
        }
      break;
      case 2:
        matrix.clear();
        displayByteImage(0x0018241020241800);
        matrix.update();

        if (btn_middle.getState() == PRESS) {
          transition_effect();
          loadGame(new Jump());
        }
      break;
      case 3:
        const uint64_t IMAGES[] = {
          0x003c424242424200, // u
          0x0024141c24241800, // r
          0x0018181818001800, // i
          0x0018181818001800, // i
          0x003c444444443c00, // d
          0x003c101014181000  // 1
        };
        const int IMAGES_LEN = sizeof(IMAGES)/8;

        if (millis() - timeout_50ms >= 300) {
          timeout_50ms = millis();
          
          matrix.clear();
          displayByteImage(IMAGES[imageIndex]);
          matrix.update();

          imageIndex++;
          if (imageIndex >= IMAGES_LEN) imageIndex = 0;
        }
      break;
    }
  }

  if (game) {
    game->update();

    // Выход из игры в меню
    if (
      btn_left.getState() == HOLD &&
      btn_right.getState() == HOLD && 
      btn_middle.getState() == HOLD
    ) {
      delete game;
      game = nullptr;
      state_menu = true;
      transition_effect();
    }
  }

  delay(10);
}

//
// Обработка нажатий
//
void keys_update() {
  btn_left.update();
  btn_right.update();
  btn_middle.update();

  if (state_menu) {
    // Выбор игры
    if (btn_left.getState() == PRESS) {
      if (gameIndex < GAME_COUNT) gameIndex++;
    } else if (btn_right.getState() == PRESS) {
      if (gameIndex > 0) gameIndex--;
    }
  }
}
