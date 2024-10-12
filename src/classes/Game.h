#ifndef GAME_H
#define GAME_H
#include <Arduino.h>
// Базовый класс
class Game {
  public:
    virtual void update() = 0;
    virtual void init() = 0;
    // Виртуальный деструктор
    virtual ~Game() {}
};
#endif
