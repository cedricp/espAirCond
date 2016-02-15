/*
 * InfraRed base class 
 * 2015 Cedric PAILLE
 */

#include "ir_send.h"

/*
 * Here I use CPU cycles count to measure time (should be more efficient than delay() method)
 * @80 Mhz : 80 cycles = 1 ms (1000ns / 12.5ns)
 */
 
#define CYCLES_USEC 80

// This inline method returns the number of CPU cycles since boot
static inline unsigned get_ticks(void)
{
    unsigned r;
    asm volatile ("rsr %0, ccount" : "=r"(r));
    return r;
}

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
  // period in cycles
  float periodcylces = (1000.f / (float)kHz) * (float)CYCLES_USEC;
  
  m_halfPeriodCycles = periodcylces / 2.0f;
  m_periodOver3Cycles = periodcylces / 3.0f;
}

// 50% duty cycle
void
ir_send::ir_on(int time)
{
  unsigned loopstop = get_ticks() + (time * CYCLES_USEC);
  while(get_ticks() < loopstop){
    unsigned sstop1 = get_ticks() + m_halfPeriodCycles;
    digitalWrite(m_gpiopin, HIGH);
    while(get_ticks() < sstop1){}
    unsigned sstop2 = get_ticks() + m_halfPeriodCycles;
    digitalWrite(m_gpiopin, LOW);
    while(get_ticks() < sstop2){}
  }
}

// 100% duty cycle
void
ir_send::ir_on_100(int time)
{
  unsigned loopstop = get_ticks() + (time * CYCLES_USEC);
  while(get_ticks() < loopstop){
    unsigned sstop1 = get_ticks() + (m_halfPeriodCycles * 2);
    digitalWrite(m_gpiopin, HIGH);
    while(get_ticks() < sstop1){}
  }
  digitalWrite(m_gpiopin, LOW);
}

// 33% duty cycle
void
ir_send::ir_on_33(int time)
{
  unsigned loopstop = get_ticks() + (time * CYCLES_USEC);
  while(get_ticks() < loopstop){
    unsigned sstop1 = get_ticks() + m_periodOver3Cycles;
    digitalWrite(m_gpiopin, HIGH);
    // 33% on
    while(get_ticks() < sstop1){}
    unsigned sstop2 = get_ticks() + (m_periodOver3Cycles * 2);
    digitalWrite(m_gpiopin, LOW);
    // 66% off
    while(get_ticks() < sstop2){}
  }
}

void
ir_send::ir_off(int time)
{
  unsigned loopstop = get_ticks() + (time * CYCLES_USEC);
  digitalWrite(m_gpiopin, LOW);
  while(get_ticks() < loopstop){}
}

void
ir_send::set_gpio_pin(int pin)
{
  m_gpiopin = pin;
  pinMode(pin, OUTPUT);
}


