#include "Stepper2.h"

Stepper2::Stepper2(int out1, int out2, int out3, int out4, boolean clockwise, unsigned long delay_us) {
  this->out1 = out1;
  this->out2 = out2;
  this->out3 = out3;
  this->out4 = out4;
  
  pinMode(this->out1, OUTPUT);
  pinMode(this->out2, OUTPUT);
  pinMode(this->out3, OUTPUT);
  pinMode(this->out4, OUTPUT);

  this->on = false;
  
  this->cw = clockwise;
  this->step = 0;

  this->step_delay = delay_us;
  this->last_step = 0;
}

boolean Stepper2::IsOn()
{
  return this->on;  
}

void Stepper2::SetOn(boolean on)
{
  if (this->on != on)
  {
    this->on = on;

    if (this->on)
    {
      this->step = 0;
      this->last_step = micros();
    }
    else
    {
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, LOW);
    }
  }
}

void Stepper2::Update() {  
  if (!this->on) return;
  
  unsigned long now = micros();
  unsigned long next = this->last_step + this->step_delay;

  if (next > this->last_step)
  { 
    if (now >= next || now < this->last_step)
    {
      this->Step();
      this->last_step = now;
    }
  }
  else
  {
    if (now >= next && now < this->last_step)
    {
      this->Step();
      this->last_step = now;
    }
  }
}

void Stepper2::Step() {
  switch (this->step) {
    case 0:
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, HIGH);
      break;
    case 1:
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, HIGH);
      digitalWrite(this->out4, HIGH);
      break;
    case 2:
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, HIGH);
      digitalWrite(this->out4, LOW);
      break;
    case 3:
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, HIGH);
      digitalWrite(this->out3, HIGH);
      digitalWrite(this->out4, LOW);
      break;
    case 4:
      digitalWrite(this->out1, LOW);
      digitalWrite(this->out2, HIGH);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, LOW);
      break;
    case 5:
      digitalWrite(this->out1, HIGH);
      digitalWrite(this->out2, HIGH);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, LOW);
      break;
    case 6:
      digitalWrite(this->out1, HIGH);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, LOW);
      break;
    case 7:
      digitalWrite(this->out1, HIGH);
      digitalWrite(this->out2, LOW);
      digitalWrite(this->out3, LOW);
      digitalWrite(this->out4, HIGH);
      break;
  }

  if (this->cw) {
    this->step++;

    if (this->step > 7)
      this->step = 0;
  }
  else
  {
    this->step--;
    
    if (this->step < 0)
      this->step = 7;
  }
}
