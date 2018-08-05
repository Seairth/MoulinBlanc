#ifndef OneShotButton_h
#define OneShotButton_h

#include <Arduino.h>

class OneShotButton
{
  public:
    OneShotButton(int pin, int pressed_when);
    boolean IsPressed();
    void Update();

  private:
    int pin;
    boolean pressed;

    int last_state;
    int pressed_when;

    int last_read_state;
    unsigned long last_debounce;
    unsigned long debounce_delay;
};

#endif
