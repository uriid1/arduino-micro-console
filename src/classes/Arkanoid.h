#ifndef ARKANOID_H
#define ARKANOID_H

class Ball {
  public:
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = -1;

    Ball(int _x, int _y) {
      x = _x;
      y = _y;
    }

    void update() {
      x += dx;
      y += dy;

      // Столкновение с границами
      if (x == W_BORDER || x == 0) dx = -dx;
      if (y == 0) dy = -dy;

      // Не даем шарику выйти за границы
      if (x > W_BORDER) {
        x = W_BORDER;
        dx = -dx;
      } else if (x < 0) {
        x = 0;
        dx = -dx;
      }
      if (y < 0) {
        y = 0;
        dy = -dy;
      }
    }

    void draw() {
      matrix.dot(x, y);
    }
};

class Desk {
  public:
    int x;
    int y;

    Desk(int _x, int _y) {
      x = _x;
      y = _y;
    }

    void update() {
      if (digitalRead(PIN_BTN_LEFT) == 0) {
        x += 1;
      } else if (digitalRead(PIN_BTN_RIGHT) == 0) {
        x -= 1;
      }

      if (x + 1 > W_BORDER) {
        x = W_BORDER - 1;
      } else if (x < -1) {
        x = -1;
      }
    }

    void draw() {
      matrix.dot(x, y);
      matrix.dot(x + 1, y);
      matrix.dot(x + 2, y);
    }
};

class Brick {
  public:
    int x;
    int y;

    Brick(int startX, int startY) {
      x = startX;
      y = startY;
    }

    void draw() {
      matrix.dot(x, y);
    }
};

// Класс игры в арканоид
class Arkanoid: public Game {
  #define MAX_BRICKS_X 8
  #define MAX_BRICKS_Y 3
  #define LOOP_DELAY 100

  private:
    unsigned long __game_loop_timeout = 0;
    bool blinked = true;
    bool followDesk = true;
    int bricksCount = 0;

    Desk* desk;
    Ball* ball;
    Brick* bricks[MAX_BRICKS_Y][MAX_BRICKS_X];

    void bricks_init() {
      bricksCount = MAX_BRICKS_Y * MAX_BRICKS_X;

      for (int y = 0; y < MAX_BRICKS_Y; y++) {
        for (int x = 0; x < MAX_BRICKS_X; x++) {
          bricks[y][x] = new Brick(x, y);
        }
      }
    }

  public:
    void init() override {
      // Объекты
      desk = new Desk(2, HEIGHT - 1);
      ball = new Ball(3, HEIGHT - 2);

      // Заполнение блоков
      bricks_init();
    }

    void update() override {
      draw();

      if (blinked) {
        for (int i = 0; i < 5; i ++) {
          matrix.clear();
          matrix.update();
          delay(150-i*20);
          draw();
          delay(150-i*20);
        }
        blinked = false;
      }

      if (millis() - __game_loop_timeout >= LOOP_DELAY) {
        __game_loop_timeout = millis();
        game_loop();
      }
    }

    void game_loop() {
      // 
      // Обновление сущностей
      //
      if (bricksCount == 0) {
        followDesk = true;
        blinked = true;
        bricks_init();
      }

      // Обновление доски
      desk->update();

      // Шарик следует за доской
      if (followDesk) {
        ball->x = desk->x + 1;
        ball->y = desk->y - 1;
        ball->dx = -1;
        ball->dy = -1;

        followDesk = digitalRead(PIN_BTN_MIDDLE);
      } else {
        ball->update();
      }

      // Шарик упал
      if (ball->y > H_BORDER) {
        followDesk = true;
        blinked = true;
      }

      // Столкновение с блоками
      for (int y = 0; y < MAX_BRICKS_Y; y++ ) {
        for (int x = 0; x < MAX_BRICKS_X; x++ ) {
          Brick* brick = bricks[y][x];

          if (brick == nullptr) {
            continue;
          }

          // Столкновение с верхней частью блока
          if (ball->y + ball->dy == brick->y && ball->x == brick->x) {
            delete bricks[y][x];
            bricks[y][x] = nullptr;

            ball->dy = -ball->dy;
            bricksCount--;
            break;

          // Столкновение с боковой частью
          } else if (ball->y == brick->y && ball->x + ball->dx == brick->x) {
            delete bricks[y][x];
            bricks[y][x] = nullptr;

            ball->dx = -ball->dx;
            bricksCount--;
            break;

          // Столкновение по горизонтали
          } else if (ball->y + ball->dy == brick->y && ball->x + ball->dx == brick->x) {
            delete bricks[y][x];
            bricks[y][x] = nullptr;

            ball->dy = -ball->dy;
            bricksCount--;
            break;
          }
        }
      }

      // Столкновение с доской
      // |*|**
      if (ball->x == desk->x || ball->x + ball->dx == desk->x) {
        if (ball->y + ball->dy == desk->y) {
          ball->dy = -1;
          ball->dx = -1;
        }
      // *|*|*
      } else if (ball->x == desk->x + 1 || ball->x + ball->dx == desk->x + 1) {
        if (ball->y + ball->dy == desk->y) {
          ball->dy = -1;
          ball->dx = 0;
        }
      // **|*|
      } else if (ball->x == desk->x + 2 || ball->x + ball->dx == desk->x + 2) {
        if (ball->y + ball->dy == desk->y) {
          ball->dy = -1;
          ball->dx = 1;
        }
      }
    }

    void draw() {
      matrix.clear();
      // Отрисовка игрока
      desk->draw();
      // Отрисовка шарика
      ball->draw();
      
      // Отрисовка блоков
      for (int y = 0; y < MAX_BRICKS_Y; y++) {
        for (int x = 0; x < MAX_BRICKS_X; x++) {
          if (bricks[y][x] != nullptr) {
            bricks[y][x]->draw();
          }
        }
      }
      matrix.update();
    }
};
#endif