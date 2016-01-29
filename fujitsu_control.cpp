/*
   Fujitsu InfraRed module
   Based on document from David Abrams
   Source : https://www.remotecentral.com
   Other useful source : Kaseikyo56 - http://www.hifi-remote.com/wiki/index.php?title=Kaseikyo
*/

#include "fujitsu_control.h"

#define MASK_MSB 0x80

#define FJTSU_AIR_MODE_HEAT  0x04
#define FJTSU_AIR_MODE_FAN   0x03
#define FJTSU_AIR_MODE_COOL  0x01
#define FJTSU_AIR_MODE_DRY   0x02
#define FJTSU_AIR_MODE_AUTO  0x00

#define FJTSU_FAN_SPEED_AUTO  0x00
#define FJTSU_FAN_SPEED_HIGH  0x01
#define FJTSU_FAN_SPEED_MID   0x02
#define FJTSU_FAN_SPEED_LOW   0x03
#define FJTSU_FAN_SPEED_QUIET 0x04

#define SWING_OFF       0x00
#define SWING_VERTICAL  0x01
#define SWING_HORZONTAL 0x02
#define SWING_BOTH      0x03

#define UNIT_TIME 412

fujitsu_contol::fujitsu_contol(int ir_pin) : aircond_control(ir_pin)
{
  ir.set_period(38);
  m_temperature  = 20;
  m_air_mode = FJTSU_AIR_MODE_HEAT;
  m_fan_mode = FJTSU_FAN_SPEED_AUTO;
  m_power_status = 0;
}

fujitsu_contol::~fujitsu_contol()
{

}

bool
fujitsu_contol::set_swing(bool h, bool v)
{
  m_swing = 0x00;
  m_swing =  h ? 0x02 : 0x00;
  m_swing |= v ? 0x01 : 0x00;
  return true;
}

bool
fujitsu_contol::set_fan_mode(fan_mode mode)
{
  switch (mode) {
    case FAN_SPEED_AUTO:
      m_fan_mode = FJTSU_FAN_SPEED_AUTO;
      break;
    case FAN_SPEED_LOW:
      m_fan_mode = FJTSU_FAN_SPEED_LOW;
      break;
    case FAN_SPEED_MID:
      m_fan_mode = FJTSU_FAN_SPEED_MID;
      break;
    case FAN_SPEED_HIGH:
      m_fan_mode = FJTSU_FAN_SPEED_HIGH;
      break;
    case FAN_SPEED_QUIET:
      m_fan_mode = FJTSU_FAN_SPEED_QUIET;
      break;
  }
  return true;
}

void
fujitsu_contol::set_power(bool on)
{
  m_power_status = on ? 1 : 0;
}

bool
fujitsu_contol::set_temperature(int temp)
{
  if (temp < 18 || temp > 31)
    return false;
  m_temperature = temp;
}

bool
fujitsu_contol::set_ac_mode(ac_mode mode)
{
  switch (mode) {
    case MODE_COOL:
      m_air_mode = FJTSU_AIR_MODE_COOL;
      break;
    case MODE_AUTO:
      m_air_mode = FJTSU_AIR_MODE_AUTO;
      break;
    case MODE_HEAT:
      m_air_mode = FJTSU_AIR_MODE_HEAT;
      break;
    case MODE_DRY:
      m_air_mode = FJTSU_AIR_MODE_DRY;
      break;
    case MODE_FAN:
      m_air_mode = FJTSU_AIR_MODE_FAN;
      break;
  }
  return true;
}


void
fujitsu_contol::set_adress(char adress)
{
  m_adress = adress;
}

void fujitsu_contol::send_leader()
{
  ir.ir_on(UNIT_TIME*8);
  ir.ir_off(UNIT_TIME*4);
}

void fujitsu_contol::send_trailer()
{
  ir.ir_on(UNIT_TIME);
  ir.ir_off(UNIT_TIME*173);
}

void fujitsu_contol::send_bit_one()
{
  ir.ir_on(UNIT_TIME);
  ir.ir_off(UNIT_TIME);
}


void fujitsu_contol::send_bit_zero()
{
  ir.ir_on(UNIT_TIME);
  ir.ir_off(UNIT_TIME*4);
}

void
fujitsu_contol::send_byte(char b)
{
  int i;
  for (i = 0; i < 8; ++i) {
    if (b & MASK_MSB) {
      send_bit_one();
    } else {
      send_bit_zero();
    }
    b <<= 1;
  }
}

char
fujitsu_contol::compute_crc(char *bytes)
{
  int i;
  int sum = 0;
  for (i = 7; i < 15; ++i) {
    sum += bytes[i];
  }
  return 0x100 - (sum & 0xff) ;
}

void
fujitsu_contol::send_data()
{
  char bytes[16], i;
  bytes[0] = 0x14;
  bytes[1] = 0x63;
  bytes[2] = 0x00;
  bytes[3] = 0x10;
  bytes[4] = 0x10;
  bytes[5] = 0xFE;
  bytes[6] = 0x09;
  bytes[7] = 0x30;
  bytes[8] = ((m_temperature - 16) << 4) | m_power_status;
  bytes[9] = m_air_mode;
  bytes[10] = m_fan_mode | (m_swing << 4);
  bytes[11] = 0x00;
  bytes[12] = 0x00;
  bytes[13] = 0x00;
  bytes[14] = 0x20;
  bytes[15] = compute_crc(bytes);

  m_power_status = 0;

  noInterrupts();
  send_leader();

  for (i = 0; i < 16; ++i) {
    send_byte(bytes[i]);
  }

  send_trailer();
  interrupts();
}

void
fujitsu_contol::poweroff()
{
  char bytes[7], i;
  bytes[0] = 0x14;
  bytes[1] = 0x63;
  bytes[2] = 0x00;
  bytes[3] = 0x10;
  bytes[4] = 0x10;
  bytes[5] = 0x02;
  bytes[6] = 0xFD;

  send_leader();

  for (i = 0; i < 7; ++i) {
    send_byte(bytes[i]);
  }

  send_trailer();
}

void
fujitsu_contol::poweron()
{
  set_power(true);
  send_data();
}

