/*
 * InfraRed base class 
 * 2015 Cedric PAILLE
 */

#include "ir_send.h"
#include <Arduino.h>

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
  m_periodOver3 = (m_halfPeriod * 2) / 3;
}

void
ir_send::ir_on(int time)
{
  long beginning = micros();
  while((micros() - beginning) < time){
    digitalWrite(m_gpiopin, HIGH);
    delayMicroseconds(m_halfPeriod);
    digitalWrite(m_gpiopin, LOW);
    delayMicroseconds(m_halfPeriod);
  }
}

void
ir_send::ir_on_2(int time)
{
  long beginning = micros();
  while((micros() - beginning) < time){
    digitalWrite(m_gpiopin, HIGH);
    delayMicroseconds(m_periodOver3);
    digitalWrite(m_gpiopin, LOW);
    delayMicroseconds(m_periodOver3*2);
  }
}

void
ir_send::ir_off(int time)
{
  digitalWrite(m_gpiopin, LOW);
  if (time > 0) delayMicroseconds(time);
}

void
ir_send::set_gpio_pin(int pin)
{
  m_gpiopin = pin;
  pinMode(pin, OUTPUT);
}


