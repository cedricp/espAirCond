#ifndef AIRTON_CONTROL_H
#define AIRTON_CONTROL_H

#include "aircond_control.h"

class airton_control : public aircond_control
{
public:
  airton_control(int ir_pin);
  ~airton_control();

  void set_adress(char adress);
  
  virtual void send_data();
  virtual void poweron();
  virtual void poweroff();
  virtual void set_power(bool on); 
  virtual bool set_temperature(int temp);
  virtual bool set_ac_mode(ac_mode mode);
  virtual bool set_fan_mode(fan_mode mode);
  virtual bool set_swing(bool horizontal, bool vertical);

private:
  void send_leader();
  void send_trailer();
  void send_bit_one();
  void send_bit_zero();
  void send_byte(char);
  char compute_crc(char *bytes);
};

#endif
