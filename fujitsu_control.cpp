/*
   Fujitsu InfraRed module
   Based on document from David Abrams
   Source : https://www.remotecentral.com
   Other useful source : Kaseikyo56 - http://www.hifi-remote.com/wiki/index.php?title=Kaseikyo
*/

#include "fujitsu_control.h"

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

fujitsu_contol::fujitsu_contol(int ir_pin, bool open_drain) : aircond_control(ir_pin, open_drain)
{
  ir.set_period(38);
}

fujitsu_contol::~fujitsu_contol()
{

}

void fujitsu_contol::send_leader()
{
  ir.ir_on_33(3280);
  ir.ir_off(1640);
}

void fujitsu_contol::send_trailer()
{
  ir.ir_on_33(410);
  ir.ir_off(800);
}

void fujitsu_contol::send_bit_zero()
{
  ir.ir_on_33(410);
  ir.ir_off(410);
}

void fujitsu_contol::send_bit_one()
{
  ir.ir_on_33(410);
  ir.ir_off(1230);
}

void
fujitsu_contol::send_byte(char b)
{
  int i;
  for (i = 0; i < 8; ++i) {
    if (b & 0x1) {
      send_bit_one();
    } else {
      send_bit_zero();
    }
    b = b >> 1;
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
  return 0x100 - (sum & 0xff);
}

void
fujitsu_contol::send_data()
{
  int swing, air_mode, fan_mode, temp;
  swing =  m_swing_h ? 0x02 : 0x00;
  swing |= m_swing_v ? 0x01 : 0x00;

  switch (m_air_mode) {
    case MODE_COOL:
      air_mode = FJTSU_AIR_MODE_COOL;
      break;
    case MODE_AUTO:
      air_mode = FJTSU_AIR_MODE_AUTO;
      break;
    case MODE_HEAT:
      air_mode = FJTSU_AIR_MODE_HEAT;
      break;
    case MODE_DRY:
      air_mode = FJTSU_AIR_MODE_DRY;
      break;
    case MODE_FAN:
      air_mode = FJTSU_AIR_MODE_FAN;
      break;
  }

  switch (m_fan_mode) {
    case FAN_SPEED_AUTO:
      fan_mode = FJTSU_FAN_SPEED_AUTO;
      break;
    case FAN_SPEED_LOW:
      fan_mode = FJTSU_FAN_SPEED_LOW;
      break;
    case FAN_SPEED_MID:
      fan_mode = FJTSU_FAN_SPEED_MID;
      break;
    case FAN_SPEED_HIGH:
      fan_mode = FJTSU_FAN_SPEED_HIGH;
      break;
    case FAN_SPEED_QUIET:
      fan_mode = FJTSU_FAN_SPEED_QUIET;
      break;
  }

  temp = m_temperature > 31 ? 31 : m_temperature;
  temp = temp < 16 ? 16 : temp;
  
  char bytes[16], i;
  bytes[0] = 0x14;
  bytes[1] = 0x63;
  bytes[2] = 0x00;
  bytes[3] = 0x10;
  bytes[4] = 0x10;
  bytes[5] = 0xFE;
  bytes[6] = 0x09;
  bytes[7] = 0x30;
  bytes[8] = ((temp - 16) << 4) | m_powermem;
  bytes[9] = air_mode;
  bytes[10] = fan_mode | (swing << 4);
  bytes[11] = 0x00;
  bytes[12] = 0x00;
  bytes[13] = 0x00;
  bytes[14] = 0x20;
  bytes[15] = compute_crc(bytes);

  m_powermem = false;

  while(!ir.can_begin_send()){
    // Don't start until we risk a clock overflow
  }

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
  set_power(false);
  m_powermem = false;
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
  m_powermem = true;
  send_data();
}

