espAirCond
==========

An **ESP8266** infrared transmiter for Airton air conditionner remote (YKRH) replacement that can be controlled with a web request and post/get data. Fujistu air conditionner IR control is also available (not tested, actually).
I made this program to control my air conditionners from my smartphone.

You can easily tweak this code for other microcontrollers.

Note:
-----

IR timings are based on CPU cycles, so if you change the CPU clock speed (80Mhz on ESP8266 by default), you'll have to tweak the ir_send class timings.

