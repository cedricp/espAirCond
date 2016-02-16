#ifndef AIRTON_CONTROL_H
#define AIRTON_CONTROL_H

#include "aircond_control.h"

class airton_control : public aircond_control
{
public:
  airton_control(int ir_pin, bool opendrain);
  ~airton_control();
  
  virtual void send_data();
  virtual void poweron();
  virtual void poweroff();

private:
  void send_leader();
  void send_trailer();
  void send_bit_one();
  void send_bit_zero();
  void send_byte(char);
  char compute_crc(char *bytes);
};

#endif
