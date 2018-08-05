#include "Arduino.h"
#include "OneShotButton.h"

OneShotButton::OneShotButton(int pin, int pressed_when) {
  debounce_delay = 50;
  
  this->pin = pin;
  
  this->pressed_when = pressed_when;
  pinMode(this->pin, INPUT);

  last_state = last_read_state = digitalRead(this->pin);

  pressed = (last_state == this->pressed_when);
}

boolean OneShotButton::IsPressed()
{
  if (pressed)
  {
    pressed = false;
    return true;
  }

  return false;
}

void OneShotButton::Update()
{
  int reading = digitalRead(pin);

  if (reading != last_read_state)
  {
    last_debounce = millis();  
  }

  if ((millis() - last_debounce) > debounce_delay)
  {
    if (reading != last_state)
    {
      last_state = reading;

      if (last_state == pressed_when)
      {
        pressed = true;
      }
    }
  }

  last_read_state = reading;
}

