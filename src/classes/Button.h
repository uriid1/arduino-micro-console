#ifndef BUTTON_H
#define BUTTON_H

enum ButtonState {
  PRESS,    // Кнопка нажата
  PRESSED,  // Кнопка удерживается
  HOLD,     // Кнопка долго удерживается
  RELEASE   // Кнопка отпущена
};

class Button {
  private:
    int pin;
    bool lastButtonState;
    unsigned long pressTime;
    ButtonState state; // Состояние кнопки
    const unsigned long pressDuration = 500;

  public:
    Button(int pin) : pin(pin), lastButtonState(LOW), pressTime(0) {
      pinMode(pin, INPUT_PULLUP);
    }

    ButtonState getState() const {
      return state;
    }

    void update() {
      bool currentButtonState = digitalRead(pin);

      // Была ли нажатие (press)
      if (currentButtonState == LOW && lastButtonState == HIGH) {
        pressTime = millis();
        state = PRESS;
      }

      // Удерживается ли кнопка (pressed)
      else if (currentButtonState == LOW) {
        if (state != HOLD) {
          state = PRESSED;
        }

        // Удерживается ли кнопка долго
        if (millis() - pressTime >= pressDuration) {
          state = HOLD; 
        }
      }

      // Кнопка отпущена
      else if (currentButtonState == HIGH && lastButtonState == LOW) {
        state = RELEASE;
      }

      lastButtonState = currentButtonState;
    }
};
#endif
