#include "LEDStrip.h"

#define LOW_BRIGHTNESS 64
#define HIGH_BRIGHTNESS 192
#define LED_STRIP_UPDATE_DELAY 100
#define LED_STRIP_PATTERN_COUNT 5

#define LED_STRIP_TIMESCALE 50
#define LED_STRIP_1_SECOND  1000 / LED_STRIP_TIMESCALE
#define LED_STRIP_2_SECONDS 2000 / LED_STRIP_TIMESCALE
#define LED_STRIP_5_SECONDS 5000 / LED_STRIP_TIMESCALE
#define LED_STRIP_8_SECONDS 8000 / LED_STRIP_TIMESCALE

const static RgbwColor led_strip_color_off(0,0,0,0);
const static RgbwColor finale_color_red(HIGH_BRIGHTNESS,0,0,0);
const static RgbwColor finale_color_red_super_bright(255,0,0,0);
const static RgbwColor finale_color_white(0,0,0,LOW_BRIGHTNESS);

LEDStrip::LEDStrip(uint8_t pin, uint8_t white_pin, uint8_t red_pin, uint16_t count) : strip(count, pin), animations(1,LED_STRIP_TIMESCALE)
{
  this->pin = pin;
  this->white_pin = white_pin;
  this->red_pin = red_pin;

  digitalWrite(this->white_pin, LOW);
  digitalWrite(this->red_pin, LOW);
  
  pinMode(this->white_pin, OUTPUT);
  pinMode(this->red_pin, OUTPUT);
  
  this->count = count;

  this->brightness = led_brightness_off;
  this->red = false;
  this->patterned = false;
  
  this->finale = -1;
  
  this->pattern_group = 0;
  this->pattern_refresh = false;

  this->SetColor();

  this->pattern = led_pattern_fade_on;
}

void LEDStrip::Setup()
{
  strip.Begin();
  strip.ClearTo(this->color);
}

bool LEDStrip::Update(bool finale)
{
  if(finale)
  {
    if(this->finale == -1)
    {
      // start of finale!
      this->StartFinale();
    }
    
    bool updating = this->UpdateFinale();

    if (!updating)
      this->EndFinale();
    
    return updating;
  }
  else
  {
    if(this->finale != -1)
    {
      // finale canceled!
      this->EndFinale();
    }

    // not in finale.  just business as usual...
    this->UpdateInternal();

    return false;
  }
}

void LEDStrip::UpdateInternal()
{
  static const led_pattern patterns[2][LED_STRIP_PATTERN_COUNT] = {
    {led_pattern_fade_off, led_pattern_snake_up_off, led_pattern_snake_down_off, led_pattern_center_in_off, led_pattern_center_out_off},
    {led_pattern_fade_on, led_pattern_snake_up_on, led_pattern_snake_down_on, led_pattern_center_in_on, led_pattern_center_out_on}
  };

  if(this->patterned && this->brightness != led_brightness_off)
  {
    if (animations.IsAnimating())
    {
      animations.UpdateAnimations();
    }
    else
    {
      this->pattern_group = (this->pattern_group + 1) % 2;
      this->pattern = patterns[this->pattern_group][random(0,LED_STRIP_PATTERN_COUNT)];
      animations.StartAnimation(0, LED_STRIP_5_SECONDS, LEDStrip::animation_callback, this);
    }
  }

  strip.Show();
}

bool LEDStrip::UpdateFinale()
{
  switch(this->finale)
  {
    case 0:
      if(this->patterned && this->brightness != led_brightness_off)
      {
        if (animations.IsAnimating())
        {
          animations.UpdateAnimations();
          break;
        }
      }

      this->SetRed(false);
      this->SetPatterned(true);
      this->SetBrightness(led_brightness_high);

      if (this->pattern_group == 1)
      {
        this->pattern = led_pattern_fade_on;
        animations.StartAnimation(0, LED_STRIP_1_SECOND, LEDStrip::animation_callback, this);
        this->pattern_group = 0;
      }
      else
      {
        this->pattern = led_pattern_finale_0;
        animations.StartAnimation(0, LED_STRIP_2_SECONDS, LEDStrip::animation_callback, this);

        this->finale++;
      }
      
      break;

    case 1:
      if (animations.IsAnimating())
      {
        animations.UpdateAnimations();
        break;
      }

      this->pattern = led_pattern_finale_1;
      animations.StartAnimation(0, LED_STRIP_8_SECONDS, LEDStrip::animation_callback, this);

      this->SetRed(true);
      
      this->finale++;
      break;

    case 2:
      if (animations.IsAnimating())
      {
        animations.UpdateAnimations();
        break;
      }

      this->finale++;
      // let it fall through to default
    
    default:
      return false;
  }

  strip.Show();
  return true;
}

led_brightness LEDStrip::GetBrightness()
{
  return this->brightness;
}

void LEDStrip::SetBrightness(led_brightness brightness)
{
  if (this->brightness != brightness)
  {
    this->brightness = brightness;
    this->SetColor();

    if(this->patterned)
    {
      if (this->brightness == led_brightness_off)
      {
        animations.StopAnimation(0);
        this->strip.ClearTo(this->color);
        this->SetMiniLEDs(false);
      }
      else
      {
         this->pattern_refresh = true;
      }
    }
    else
    {
      strip.ClearTo(this->color);
      this->SetMiniLEDs(this->brightness != led_brightness_off);
    }
  }
}

bool LEDStrip::IsRed()
{
  return this->red;
}

void LEDStrip::SetRed(bool red)
{
  if (this->red != red)
  {
    this->red = red;
    this->SetColor();

    if(this->brightness != led_brightness_off)
    {
      if(this->patterned)
      {
        this->strip.ClearTo((pattern_group == 0) ? this->color : led_strip_color_off);
        this->SetMiniLEDs(this->pattern_group == 0);
      }
      else
      {
        strip.ClearTo(this->color);
        this->SetMiniLEDs(true);
      }
    }
  }
}

bool LEDStrip::IsPatterned()
{
  return this->patterned;
}

void LEDStrip::SetPatterned(bool patterned)
{
  if(this->patterned != patterned)
  {
    this->patterned = patterned;

    if(this->patterned)
    {
      this->pattern_group = (this->brightness == led_brightness_off) ? 0 : 1;

      this->pattern_refresh = true;
    }
    else
    {
      animations.StopAnimation(0);

      strip.ClearTo(this->color);
      
      this->SetMiniLEDs((this->brightness != led_brightness_off));        
    }
  }
}

void LEDStrip::SetColor()
{
  if (this->brightness == led_brightness_off)
  {
    this->color = led_strip_color_off;
  }
  else
  {
    uint8_t brightness = (this->brightness == led_brightness_low) ? LOW_BRIGHTNESS : HIGH_BRIGHTNESS;
    
    if (this->red)
      this->color = RgbwColor(brightness,0,0,0);
    else
      this->color = RgbwColor(0,0,0,brightness);
  }
}

void LEDStrip::SetMiniLEDs(bool on)
{
  if(on)
  {
    analogWrite(this->red?this->red_pin:this->white_pin, (this->brightness == led_brightness_low) ? LOW_BRIGHTNESS : HIGH_BRIGHTNESS);
    analogWrite(this->red?this->white_pin:this->red_pin, 0);
  }
  else
  {
    analogWrite(this->white_pin, 0);
    analogWrite(this->red_pin, 0);
  }
}

void LEDStrip::StartFinale()
{
  finale_save_brightness = this->brightness;
  finale_save_red = this->red;
  finale_save_pattern = this->patterned;

  this->finale = 0;
}

void LEDStrip::EndFinale()
{
  this->SetBrightness(finale_save_brightness);
  this->SetRed(finale_save_red);
  this->SetPatterned(finale_save_pattern);
  
  animations.StopAnimation(0);

  this->finale = -1;
}

void LEDStrip::UpdateAnimation(const AnimationParam& param)
{
  // for the start and complete states, make sure we are at a know state
  if (param.state == AnimationState_Started || this->pattern_refresh)
  {
    this->strip.ClearTo((pattern_group == 0) ? this->color : led_strip_color_off);
    this->SetMiniLEDs((pattern_group == 0) ? true : false);
    
    this->pattern_refresh = false;
  }
  else if (param.state == AnimationState_Completed)
  {
    this->strip.ClearTo((pattern_group == 0) ? led_strip_color_off : this->color);
    this->SetMiniLEDs((pattern_group == 0) ? false : true);
  }
  
  switch(this->pattern)
  {
    case led_pattern_fade_on:         this->UpdateFadeIn(param);        break;
    case led_pattern_fade_off:        this->UpdateFadeOut(param);       break;
    case led_pattern_snake_up_on:     this->UpdateSnakeUpOn(param);     break;
    case led_pattern_snake_up_off:    this->UpdateSnakeUpOff(param);    break;
    case led_pattern_snake_down_on:   this->UpdateSnakeDownOn(param);   break;
    case led_pattern_snake_down_off:  this->UpdateSnakeDownOff(param);  break;
    case led_pattern_center_in_on:    this->UpdateCenterInOn(param);    break;
    case led_pattern_center_in_off:   this->UpdateCenterInOff(param);   break;
    case led_pattern_center_out_on:   this->UpdateCenterOutOn(param);   break;
    case led_pattern_center_out_off:  this->UpdateCenterOutOff(param);  break;

    case led_pattern_finale_0:        this->UpdateFinale0(param);       break;
    case led_pattern_finale_1:        this->UpdateFinale1(param);       break;
  }
}

void LEDStrip::UpdateFadeIn(const AnimationParam& param)
{
  RgbwColor updatedColor = RgbwColor::LinearBlend(led_strip_color_off, this->color, param.progress);

  this->strip.ClearTo(updatedColor);
}

void LEDStrip::UpdateFadeOut(const AnimationParam& param)
{
  RgbwColor updatedColor = RgbwColor::LinearBlend(this->color, led_strip_color_off, param.progress);

  this->strip.ClearTo(updatedColor);
}

void LEDStrip::UpdateSnakeUpOn(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress);

  for(int index = 0; index < progress; index++)
    this->strip.SetPixelColor(index, this->color);
}

void LEDStrip::UpdateSnakeUpOff(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress);

  for(int index = 0; index < progress; index++)
    this->strip.SetPixelColor(index, led_strip_color_off);
}

void LEDStrip::UpdateSnakeDownOn(const AnimationParam& param)
{
  int progress = this->count - (int)(this->count * param.progress);

  for(int index = this->count - 1; index >= progress; index--)
    this->strip.SetPixelColor(index, this->color);
}

void LEDStrip::UpdateSnakeDownOff(const AnimationParam& param)
{
  int progress = this->count - (int)(this->count * param.progress);

  for(int index = this->count - 1; index >= progress; index--)
    this->strip.SetPixelColor(index, led_strip_color_off);
}

void LEDStrip::UpdateCenterInOn(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress) >> 1;

  for(int index = 0; index < progress; index++)
  {
    this->strip.SetPixelColor(index, this->color);  
    this->strip.SetPixelColor(this->count - index - 1, this->color);  
  }
}

void LEDStrip::UpdateCenterInOff(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress) >> 1;

  for(int index = 0; index < progress; index++)
  {
    this->strip.SetPixelColor(index, led_strip_color_off);  
    this->strip.SetPixelColor(this->count - index - 1, led_strip_color_off);  
  }
}

void LEDStrip::UpdateCenterOutOn(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress);
  int offset = (this->count - progress) >> 1;

  for(int index = 0; index < progress; index++)
  {
    this->strip.SetPixelColor(offset + index, this->color);  
  }
}

void LEDStrip::UpdateCenterOutOff(const AnimationParam& param)
{
  int progress = (int)(this->count * param.progress);
  int offset = (this->count - progress) >> 1;

  for(int index = 0; index < progress; index++)
  {
    this->strip.SetPixelColor(offset + index, led_strip_color_off);  
  }
}

void LEDStrip::UpdateFinale0(const AnimationParam& param)
{
  int progress = this->count - (int)(this->count * param.progress);

  this->strip.ClearTo(led_strip_color_off);

  for(int index = 0; index < 15; index++)
  {
    if (index + progress < this->count)
      this->strip.SetPixelColor(index + progress, finale_color_red_super_bright);
  }
}

void LEDStrip::UpdateFinale1(const AnimationParam& param)
{
  if (param.state == AnimationState_Started)
    this->strip.ClearTo(finale_color_red);

  /*
  int progress = (int)(20 * param.progress);

  this->strip.ClearTo(finale_color_red);

  for(int index = progress % 4; index < this->count; index+=4)
  {
    this->strip.SetPixelColor(index, finale_color_white);
  }
  */ 
}
