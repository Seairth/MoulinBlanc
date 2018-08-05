# MoulinBlanc

This project involves a 3D printed "Moulin Rouge" that is instead "Blanc".  Internally, there is a bit of hardware to animate things:

* Adafruit Pro Trinket 5V, which controls everything
* LED light strip, which is used to light the structure from the inside
* LED light strings for lighting the blades
* Stepper motor for turning the blades
* Some buttons to turn various elements on and off

> NOTE: You must use the modified version of NeoPixelBus ([original by Makuna](https://github.com/Makuna/NeoPixelBus)) in this repository (place the "NeoPixelBus_by_Makuna" folder in your Arduino "libraries" folder), which adds support for method callbacks in NeoPixelAnimator. I'll eventually do a PR to the original, but don't have the time at the moment.
