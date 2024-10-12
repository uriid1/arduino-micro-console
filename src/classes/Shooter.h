#ifndef SHOOTER_H
#define SHOOTER_H

class Bullet {
  public:
    int x;
    int y;

  Bullet(int startX, int startY) {
    x = startX;
    y = startY;
  }

  void update() {
    y -= 1;
  }

  void draw() {
    matrix.dot(x, y);
  }

  bool isOffScreen() {
    return (y < 0);
  }
};

class Player {
  private:
    unsigned long fireTimeOut = 0;

  public:
    int x;
    int y;

  Player(int startX, int startY) {
    x = startX;
    y = startY;
  }

  bool isFire(int time_out) {
    if (millis() - fireTimeOut >= time_out) {
      fireTimeOut = millis();
      return true;
    }
    return false;
  }

  void update() {
    // Управление игроком
    if (digitalRead(PIN_BTN_LEFT) == 0) {
      x += 1;
    } else if (digitalRead(PIN_BTN_RIGHT) == 0) {
      x -= 1;
    }

    // Обработка столкновений
    if (x + 1 > WIDTH - 1) {
      x = WIDTH - 1;
    } else if (x - 1 < 0) {
      x = 0;
    }
  }

  void draw() {
    matrix.dot(x, y - 1);
    matrix.dot(x + 1, y);
    matrix.dot(x - 1, y);
  }
};

class Enemy {
  public:
    int x;
    float y;

  Enemy(int startX, int startY) {
    x = startX;
    y = startY;
  }

  void update() {
    y += 0.1f;
  }

  bool isOffScreen() {
    return (y > HEIGHT);
  }

  void draw() {
    matrix.dot(x, floor(y));
  }
};

// Класс игры
class Shooter: public Game {
  #define LOOP_DELAY 100

  private:
    unsigned long __game_loop_timeout = 0;
    bool blinked = true;

    Player* player;

    #define MAX_BULLETS 5
    #define MAX_ENEMIES 4

    // Массив для хранения объектов пуль
    int bulletCount = 0;
    Bullet* bullets[MAX_BULLETS];

    // Массив для хранения объектов пуль
    int enemyCount = 0;
    Enemy* enemies[MAX_ENEMIES];

    // Менеджер спавна врагов
    unsigned long swawnTimeOut = 0;
    void enemy_spawn(int time_out) {
      // Прошло ли больше X миллисекунд
      if (millis() - swawnTimeOut >= time_out) {
        swawnTimeOut = millis();

        // Спавн
        if (enemyCount < MAX_ENEMIES) {
          // Избегаем спавна двух врагов в одном
          int x = random(0, WIDTH - 1);
          int y = -1;

          for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] != nullptr) {
              if (enemies[i]->x == x && enemies[i]->y == y) {
                return NULL;
              }
            }
          }

          enemies[enemyCount] = new Enemy(x, y);
          enemyCount++;
        }
      }
    }

  public:
    void init() override {
      // Объекты
      player = new Player(4, HEIGHT - 1);
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
      enemy_spawn(1000);

      player->update();

      // Обработка пулей
      for (int i = 0; i < bulletCount; i++) {
        if (bullets[i] != nullptr) {
          bullets[i]->update();

          // Очистка если вышли за границу
          if (bullets[i]->isOffScreen()) {
            delete bullets[i];
            bullets[i] = nullptr;

            // Смещаем оставшиеся пули в массиве
            for (int j = i; j < bulletCount - 1; j++) {
              bullets[j] = bullets[j + 1];
            }
            bulletCount--;
          }
        }
      }

      // Обработка врагов
      for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] != nullptr) {
          enemies[i]->update();

          // Столкновение с пулей
          for (int bullet_index = 0; bullet_index < bulletCount; bullet_index++) {
            if (bullets[bullet_index] != nullptr) {
              if (bullets[bullet_index]->x == enemies[i]->x &&
                (bullets[bullet_index]->y == floor(enemies[i]->y) || bullets[bullet_index]->y - 1 == floor(enemies[i]->y))
              ) {
                // Удаляем пулю
                delete bullets[bullet_index];
                bullets[bullet_index] = nullptr;

                // Смещаем оставшиеся пули в массиве
                for (int j = i; j < bulletCount - 1; j++) {
                  bullets[j] = bullets[j + 1];
                }
                bulletCount--;

                // Удаляем врага
                delete enemies[i];
                enemies[i] = nullptr;

                for (int j = i; j < enemyCount - 1; j++) {
                  enemies[j] = enemies[j + 1];
                }
                enemyCount--;

                break;
              }
            }
          }

          // Очистка если вышли за границу
          if (enemies[i]->isOffScreen()) {
            delete enemies[i];
            enemies[i] = nullptr;

            // Смещаем врагов в массиве
            for (int j = i; j < enemyCount - 1; j++) {
              enemies[j] = enemies[j + 1];
            }
            enemyCount--;
            break;
          }
        }
      }

      // Проверяем нажатие кнопки fire
      if (digitalRead(PIN_BTN_MIDDLE) == 0 && bulletCount < MAX_BULLETS) {
        if (player->isFire(500)) {
          // Создаем новый объект пули и добавляем его в массив
          bullets[bulletCount] = new Bullet(player->x, player->y - 1);
          bulletCount++;
        }
      }
    }

    void draw() {
      matrix.clear();

      // Отрисовка игрока
      player->draw();

      // Отрисовка пуль
      for (int i = 0; i < bulletCount; i++) {
        if (bullets[i] != nullptr) {
          bullets[i]->draw();
        }
      }

      // Отрисовка врагов
      for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] != nullptr) {
          enemies[i]->draw();
        }
      }

      matrix.update();
    }
};
#endif