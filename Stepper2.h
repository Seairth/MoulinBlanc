#ifndef Stepper2_h
#define Stepper2_h

#include <Arduino.h>

class Stepper2
{
    public:
        Stepper2(int out1, int out2, int out3, int out4, boolean clockwise, unsigned long delay_us);

        void Update();

        boolean IsOn();

        void SetOn(boolean on);

    private:
        int out1;
        int out2;
        int out3;
        int out4;

        boolean on;

        boolean cw;
        int step;

        unsigned long step_delay;
        unsigned long last_step;

        void Step();

};

#endif
