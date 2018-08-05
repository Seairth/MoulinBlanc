#ifndef LEDStrip_h
#define LEDStrip_h

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

enum led_brightness
{
  led_brightness_off = 0,
  led_brightness_low = 1,
  led_brightness_high = 2 
};

enum led_pattern
{
  led_pattern_fade_on,
  led_pattern_fade_off,
  led_pattern_snake_up_on,
  led_pattern_snake_up_off,
  led_pattern_snake_down_on,
  led_pattern_snake_down_off,
  led_pattern_center_out_on,
  led_pattern_center_out_off,
  led_pattern_center_in_on,
  led_pattern_center_in_off
};

class LEDStrip
{
  public:
    LEDStrip(uint8_t pin, uint8_t white_pin, uint8_t red_pin, uint16_t count);
    void Setup();
    void Update();

    led_brightness GetBrightness();
    void SetBrightness(led_brightness brightness);

    bool IsRed();
    void SetRed(bool red);

    bool IsPatterned();
    void SetPatterned(bool patterned);
    
  private:
    uint8_t pin;
    uint8_t red_pin;
    uint8_t white_pin;
    
    uint16_t count;

    NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip;
    NeoPixelAnimator animations;

    RgbwColor color;

    led_brightness brightness;
    bool red;
    bool patterned;

    uint8_t pattern_group;    
    led_pattern pattern;
    bool pattern_refresh;

    void SetColor();

    void SetMiniLEDs(bool on);

    void UpdateAnimation(const AnimationParam& param);
    void UpdateFadeIn(const AnimationParam& param);
    void UpdateFadeOut(const AnimationParam& param);
    void UpdateSnakeUpOn(const AnimationParam& param);
    void UpdateSnakeUpOff(const AnimationParam& param);
    void UpdateSnakeDownOn(const AnimationParam& param);
    void UpdateSnakeDownOff(const AnimationParam& param);
    void UpdateCenterInOn(const AnimationParam& param);
    void UpdateCenterInOff(const AnimationParam& param);
    void UpdateCenterOutOn(const AnimationParam& param);
    void UpdateCenterOutOff(const AnimationParam& param);

    static void animation_callback(const AnimationParam& param, const void * data)
    {
      ((LEDStrip*)data)->UpdateAnimation(param);
    }

};

#endif

