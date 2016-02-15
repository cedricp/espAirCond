/*
 * Airton InfraRed module
 * For YKRH remote control replacement
 * Cedric PAILLE 2016
 */

#include "airton_control.h"

#define AIRTON_MODE_HEAT 0x80
#define AIRTON_MODE_COOL 0x30
#define AIRTON_MODE_DRY  0x40
#define AIRTON_MODE_AUTO 0x10
#define AIRTON_MODE_FAN  0x70

#define AIRTON_FAN_SPEED_AUTO 0b010
#define AIRTON_FAN_SPEED_HIGH 0b101
#define AIRTON_FAN_SPEED_LOW  0b110
#define AIRTON_FAN_SPEED_MID  0b111

#define AIRTON_SWING_ON 0x00
#define AIRTON_SWING_OFF 0XA0

#define POWER_ON  0x20
#define POWER_OFF 0x00 

airton_control::airton_control(int ir_pin) : aircond_control(ir_pin)
{
  ir.set_period(38);
  m_temperature  = 19;
  m_power_status = POWER_ON;
  m_air_mode = AIRTON_MODE_HEAT;
  m_fan_mode = AIRTON_FAN_SPEED_AUTO;
}

airton_control::~airton_control()
{
  
}

void airton_control::set_power(bool on)
{
  m_power_status = on ? POWER_ON : POWER_OFF;
}

void airton_control::poweron()
{
  set_power(true);
  send_data();
}

void airton_control::poweroff()
{
  set_power(false);
  send_data();
}

bool
airton_control::set_temperature(int temp)
{
  if (temp < 16 || temp > 28)
    return false;
  m_temperature = temp;
  return true;
}


bool
airton_control::set_swing(bool horizontal, bool vertical)
{
  return false;
}

bool
airton_control::set_ac_mode(ac_mode mode)
{
  switch(mode){
    case MODE_COOL:
      m_air_mode = AIRTON_MODE_COOL;
      break;
    case MODE_HEAT:
      m_air_mode = AIRTON_MODE_HEAT;
      break;
    case MODE_DRY:
      m_air_mode = AIRTON_MODE_DRY;
      break;
    case MODE_AUTO:
      m_air_mode = AIRTON_MODE_AUTO;
      break;
    case MODE_FAN:
      m_air_mode = AIRTON_MODE_FAN;
      break;
    default:
      return false;
  }
  return true;
}

void
airton_control::send_leader()
{
  ir.ir_on_33(8100);
  ir.ir_off(4400);
}

void
airton_control::send_trailer()
{
  ir.ir_on_33(650);
  ir.ir_off(5000);
}

void
airton_control::send_bit_zero()
{
  ir.ir_on_33(650);
  ir.ir_off(450);
}

void
airton_control::send_bit_one()
{
  ir.ir_on_33(650);
  ir.ir_off(1550);
}

bool
airton_control::set_fan_mode(fan_mode mode)
{
  switch(mode){
  case FAN_SPEED_AUTO:
    m_fan_mode = AIRTON_FAN_SPEED_AUTO;
    break;
  case FAN_SPEED_LOW:
    m_fan_mode = AIRTON_FAN_SPEED_LOW;
    break;
  case FAN_SPEED_MID:
    m_fan_mode = AIRTON_FAN_SPEED_MID;
    break;
  case FAN_SPEED_HIGH:
    m_fan_mode = AIRTON_FAN_SPEED_HIGH;
    break;
  default:
    return false;
  }
  return true;
}

void
airton_control::send_byte(char b)
{
  int i;
  for (i = 0; i < 8; ++i){
    if (b & 0x01)
      send_bit_one();
    else
      send_bit_zero();
      
    b = b >> 1;
  }
}

char
airton_control::compute_crc(char *bytes)
{
  int i;
  int sum = 0;
  for (i = 0; i < 12; ++i){
    sum += bytes[i];
  }
  return sum & 0xFF;
}

void
airton_control::send_data()
{
  char bytes[13], i;
  // Device ID
  bytes[0] = 0xC3;
  bytes[1] = ((m_temperature - 8) << 3);
  bytes[2] = 0x00;
  bytes[3] = 0x00;
  // Fan mode setting
  bytes[4] = m_fan_mode;
  bytes[5] = 0x00;
  bytes[6] = m_air_mode;
  bytes[7] = 0x00;
  bytes[8] = 0x00;
  bytes[9] = m_power_status;
  bytes[10] = 0x00;
  bytes[11] = 0x01;
  bytes[12] = compute_crc(bytes);
  
  noInterrupts(); 
  send_leader();

  for (i = 0; i < 13; ++i){
    send_byte(bytes[i]);
  }

  send_trailer();
  interrupts();
}

