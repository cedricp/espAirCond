#ifndef IRSEND_H
#define IRSEND_H

#include <Arduino.h>

static inline unsigned get_ticks(void)
{
    unsigned r;
    asm volatile ("rsr %0, ccount" : "=r"(r));
    return r;
}

class ir_send
{
public:
  ir_send(int pin = 0);
  ~ir_send();

  void set_gpio_pin(int pin);
  void set_period(int kHz);
  void ir_on(int time);
  void ir_on_33(int time);
  void ir_on_100(int time);
  void ir_off(int time);
  
private:
  unsigned m_halfPeriodCycles;
  unsigned m_periodOver3Cycles;
  int m_gpiopin;
};

#endif
