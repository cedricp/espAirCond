/*
 * InfraRed base class 
 * 2015 Cedric PAILLE
 */

#include "ir_send.h"

ir_send::ir_send(int pin)
{
  m_gpiopin = pin;
}

ir_send::~ir_send()
{

}

void
ir_send::set_period(int kHz)
{
  m_halfPeriod = 500/kHz;
  m_period = 1000/kHz;
  float ratio = 1000 / 12.5;
  m_ratio = (unsigned)ratio;
  m_halfPeriodCount = m_halfPeriod * m_ratio;
}

void
ir_send::set_gpio_pin(int pin)
{
  m_gpiopin = pin;
  pinMode(pin, OUTPUT);
}


