#include "OneShotButton.h"
#include "Stepper2.h"
#include "LEDStrip.h"

#define BUTTON_CENTER 9
#define BUTTON_UP     10
#define BUTTON_LEFT   11
#define BUTTON_DOWN   12
#define BUTTON_RIGHT  A1

OneShotButton toggleStepper(BUTTON_UP,LOW);
OneShotButton toggleBrightness(BUTTON_DOWN,LOW);
OneShotButton togglePattern(BUTTON_LEFT,LOW);
OneShotButton toggleColor(BUTTON_RIGHT,LOW);
OneShotButton showFinale(BUTTON_CENTER,LOW);

Stepper2 stepper(A2,A3,A4,A5,true,2000);

LEDStrip strip(8,3,6,132);

bool finale = false;
bool finale_save_stepper;

void setup() {
  SetRandomSeed();

  strip.Setup();
}

void loop() {
  showFinale.Update();

  if (finale)
  {
    if (showFinale.IsPressed())
    {
      EndFinale();
    }
  }
  else
  {
    if (showFinale.IsPressed())
    {
      StartFinale();
    }
    else
    {
      updateStepperState();
      updateStripState();
    }
  }

  if (! strip.Update(finale))
  {
    if (finale)
    {
      EndFinale();
    }
  }
  
  stepper.Update();
}

void updateStepperState() {
  toggleStepper.Update();
  
  if (toggleStepper.IsPressed())
    stepper.SetOn(!stepper.IsOn());
}

void updateStripState() {  
  toggleBrightness.Update();
  togglePattern.Update();
  toggleColor.Update();

  if (toggleBrightness.IsPressed())
  {
    switch(strip.GetBrightness())
    {
      case led_brightness_off:
        strip.SetBrightness(led_brightness_low);
        break;
      case led_brightness_low:
        strip.SetBrightness(led_brightness_high);
        break;
      default:
        strip.SetBrightness(led_brightness_off);
    }
  }
  
  if (togglePattern.IsPressed())
  {
    strip.SetPatterned(!strip.IsPatterned());
  }

  if (toggleColor.IsPressed())
  {
    strip.SetRed(!strip.IsRed());
  }
}

void StartFinale()
{
  finale_save_stepper = stepper.IsOn();

  stepper.SetOn(true);

  finale = true;
}

void EndFinale()
{
  stepper.SetOn(finale_save_stepper);

  finale = false;
}

void SetRandomSeed()
{
  uint32_t seed;

  // random works best with a seed that can use 31 bits
  // analogRead on a unconnected pin tends toward less than four bits
  seed = analogRead(0);
  delay(1);

  for (int shifts = 3; shifts < 31; shifts += 3)
  {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }

  randomSeed(seed);
}

