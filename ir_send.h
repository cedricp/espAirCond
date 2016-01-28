#ifndef IRSEND_H
#define IRSEND_H

class ir_send
{
public:
  ir_send(int pin = 0);
  ~ir_send();

  void set_gpio_pin(int pin);
  void set_period(int kHz);
  void ir_on(int time);
  void ir_off(int time);
  
private:
  int m_halfPeriod;
  int m_gpiopin;
};

#endif
