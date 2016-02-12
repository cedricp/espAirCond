#ifndef FUJITSU_CONTROL_H
#define FUJITSU_CONTROL_H

#include "aircond_control.h"

class fujitsu_contol : public aircond_control
{
public:
  fujitsu_contol(int ir_pin);
  ~fujitsu_contol();

  virtual void send_data();
  virtual void set_power(bool on); 
  virtual void poweroff();
  virtual void poweron();
  
  virtual bool set_temperature(int temp);
  virtual bool set_ac_mode(ac_mode mode);
  virtual bool set_swing(bool horizontal, bool vertical);
  virtual bool set_fan_mode(fan_mode mode);

private:
  void send_leader();
  void send_trailer();
  void send_bit_one();
  void send_bit_zero();
  void send_byte(char);
  char compute_crc(char *bytes);
};

#endif
