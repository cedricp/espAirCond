#ifndef FUJITSU_CONTROL_H
#define FUJITSU_CONTROL_H

#include "aircond_control.h"

class fujitsu_control : public aircond_control
{
public:
  fujitsu_control(int ir_pin, bool opendrain);
  ~fujitsu_control();

  virtual void send_data();
  virtual void poweroff();
  virtual void poweron();

private:
  void send_leader();
  void send_trailer();
  void send_bit_one();
  void send_bit_zero();
  void send_byte(char);
  char compute_crc(char *bytes);
  bool m_powermem;
};

#endif
