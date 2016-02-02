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
  
  inline void ir_on(int time){
    unsigned time0 = get_ticks() + (time * m_ratio);
    register unsigned time1, time2; 
    while(get_ticks() < time0){
      time1 = get_ticks() + m_halfPeriodCount;
      time2 = time1 + m_halfPeriodCount;
      digitalWrite(m_gpiopin, HIGH);
      while(get_ticks() < time1){}
      digitalWrite(m_gpiopin, LOW);
      while(get_ticks() < time2){}
    }
  }
  
  inline void ir_off(int time){
    digitalWrite(m_gpiopin, LOW);
    register unsigned time0 = get_ticks() + (time * m_ratio);
    while(get_ticks() < time0){}
  }
  
private:
  unsigned m_halfPeriodCount;
  unsigned m_ratio;
  unsigned m_gpiopin;
};

#endif
