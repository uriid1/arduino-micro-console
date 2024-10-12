#ifndef JUMP_H
#define JUMP_H

class JumpPlayer {
  private:
    byte jumpCount = 0;

  public:
    bool isJump = false;
    int x = 4;
    int y = 5;

  void update() {
    if (digitalRead(PIN_BTN_LEFT) == 0) {
      x += 1;
    } else if (digitalRead(PIN_BTN_RIGHT) == 0) {
      x -= 1;
    }

    if (y >= H_BORDER) {
      jumpCount = 0;
      isJump = true;
    }

    if (isJump && jumpCount < 4) {
      jumpCount++;
      y--;
      if (jumpCount >= 4) {
        isJump = false;
        jumpCount = 0;
      }
    }

    if (!isJump) y += 1;

    // Отзеркаливание позиции
    if (x > W_BORDER) x = 0;
    if (x < 0) x = W_BORDER;
  }

  void draw() {
    matrix.dot(x, y);
  }
};

class Platform {
  public:
    int x;
    int y;

  Platform(int _x, int _y) {
    x = _x;
    y = _y;
  }

  void update() {
  }

  void draw() {
    matrix.dot(x, y);
    matrix.dot(x + 1, y);
    matrix.dot(x + 2, y);
  }
};

// Класс игры
class Jump: public Game {
  #define LOOP_DELAY 100
  #define MAX_PLATFORMS 4

  private:
    unsigned long __game_loop_timeout = 0;
    bool blinked = true;

    JumpPlayer* player;
    Platform* platforms[MAX_PLATFORMS];

    int platformCount = 0;

    void platform_gen() {

    }
  public:
    void init() override {
      player = new JumpPlayer();

      for (int i = 0; i < MAX_PLATFORMS; i++) {
        platforms[i] = new Platform(random(0, W_BORDER-2), H_BORDER - i*3);
        platformCount++;
      }
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
      for (int i = 0; i < platformCount; i++) {
        if (!platforms[i]) continue;

        if (!player->isJump) {
          if (player->y + 1 == platforms[i]->y &&
            (player->x == platforms[i]->x
            || player->x == platforms[i]->x + 1
            || player->x == platforms[i]->x + 2)
          ) {
            player->y = platforms[i]->y - 1;
            player->isJump = true;
          }

          // Удаление платформы
          if (platforms[i]->y > H_BORDER) {
            delete platforms[i];
            platforms[i] = nullptr;

            // Смещение всех существующих платформ влево
            // с сохранением порядка
            int null_count = 0;
            for (int i = 0; i < MAX_PLATFORMS; i++) {
              if (platforms[i] == nullptr) {
                null_count++;
              } else if (null_count > 0) {
                platforms[i], platforms[i-null_count] = platforms[i-null_count], platforms[i];
              }
            }

            platformCount--;
          }
        }
      }

      // Смещение платформ
      if (player->y < 3) {
        for (int i = 0; i < MAX_PLATFORMS; i++) {
          if (platforms[i]) platforms[i]->y++;
        }
      }
      if (player->y < 0) player->y = 0;

      // Генерируем новую платформу
      if (platformCount < MAX_PLATFORMS) {
        int mix_y = 8;

        int null_index = 0;
        int nulls[MAX_PLATFORMS];
        for (int i = 0; i < MAX_PLATFORMS; i++) {
          if (platforms[i] == nullptr) {
            nulls[null_index] = i;
            null_index++;
            continue;
          }
          if (platforms[i]->y < mix_y) mix_y = platforms[i]->y;
        }

        for (int i = 0; i < null_index; i++) {
          platforms[nulls[i]] = new Platform(random(0, W_BORDER-2), mix_y - 3*(i+1));
          platformCount++;
        }
      }

      player->update();
    }

    void draw() {
      matrix.clear();

      player->draw();

      // Платформы
      for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (platforms[i]) {
          platforms[i]->draw();
        }
      }

      matrix.update();
    }
};

#endif