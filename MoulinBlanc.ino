#include "OneShotButton.h"
#include "Stepper2.h"
#include "LEDStrip.h"

OneShotButton toggleStepper(10,LOW);
OneShotButton toggleBrightness(11,LOW);
OneShotButton togglePattern(12,LOW);
OneShotButton toggleColor(A1,LOW);

Stepper2 stepper(A2,A3,A4,A5,true,2000);

LEDStrip strip(8,3,6,132);

void setup() {
  SetRandomSeed();

  strip.Setup();
}

void loop() {
  updateStepper();
  updateStrip();
}

void updateStepper() {
  toggleStepper.Update();
  
  if (toggleStepper.IsPressed())
    stepper.SetOn(!stepper.IsOn());

  stepper.Update();
}

void updateStrip() {  
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

  strip.Update();
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

