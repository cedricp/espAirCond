#ifndef AIRCOND_CONTROL_H
#define AIRCOND_CONTROL_H

#include "ir_send.h"
#include <Arduino.h>

enum fan_mode{
  FAN_SPEED_AUTO = 0,
  FAN_SPEED_LOW,
  FAN_SPEED_MID,
  FAN_SPEED_HIGH,
  FAN_SPEED_QUIET
};

enum ac_mode{
  MODE_COOL,
  MODE_HEAT,
  MODE_FAN,
  MODE_DRY,
  MODE_AUTO
};

class aircond_control
{
public:
  aircond_control(int ir_pin){ir.set_gpio_pin(ir_pin);}
  virtual ~aircond_control(){};

  virtual void send_data() = 0;
  virtual void poweroff() = 0;
  virtual void poweron() = 0; 
  virtual void set_power(bool on) = 0; 
  virtual bool set_temperature(int temp) = 0;
  virtual bool set_ac_mode(ac_mode mode) = 0;
  virtual bool set_swing(bool horizontal, bool vertical) = 0;
  virtual bool set_fan_mode(fan_mode mode) = 0;
  String get_as_json(float curr_temp, float curr_hum, const char* status);
protected:
  ir_send ir;
  char m_temperature;
  char m_power_status;
  char m_air_mode;
  char m_fan_mode;
  char m_adress;
  char m_swing;
};

#endif
