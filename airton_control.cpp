/*
 * Airton InfraRed module
 * For YKRH remote control replacement
 * Cedric PAILLE 2016
 */

#include "airton_control.h"

#define MASK_MSB 0x80
#define TEMPERATURE_OFFSET 0x4C

#define AIRTON_MODE_HEAT 0x01 << 4
#define AIRTON_MODE_COOL 0x02 << 4
#define AIRTON_MODE_DRY  0x03 << 4

#define AIRTON_FAN_SPEED_AUTO 0x80
#define AIRTON_FAN_SPEED_HIGH 0x01
#define AIRTON_FAN_SPEED_LOW  0x02
#define AIRTON_FAN_SPEED_MID  0x03

#define POWER_ON  0x01
#define POWER_OFF 0x00 

airton_control::airton_control(int ir_pin) : aircond_control(ir_pin)
{
  ir.set_period(38);
  m_temperature  = 20;
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
  if (temp < 16 || temp > 31)
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
    default:
      return false;
  }
  return true;
}

void
airton_control::send_leader()
{
  ir.ir_on_33(8100);
  ir.ir_off(4000);
}

void
airton_control::send_trailer()
{
  ir.ir_on_33(550);
  ir.ir_off(5000);
}

void
airton_control::send_bit_zero()
{
  ir.ir_on_33(550);
  ir.ir_off(550);
}

void
airton_control::send_bit_one()
{
  ir.ir_on_33(550);
  ir.ir_off(1450);
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
    if (b & MASK_MSB)
      send_bit_one();
    else
      send_bit_zero();
      
    b <<= 1;
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
  // Device ID ??
  bytes[0] = 0xC3;
  bytes[1] = m_temperature + TEMPERATURE_OFFSET;
  bytes[2] = 0x00;
  bytes[3] = 0x00;
  bytes[4] = 0xA0;
  bytes[5] = 0x00;
  // Fan mode setting
  bytes[6] = 0x80;
  bytes[7] = 0x00;
  bytes[8] = 0x00;
  // Mode setting
  bytes[9] = 0x20;
  bytes[10] = 0x00;
  bytes[11] = m_power_status;
  bytes[12] = compute_crc(bytes);
  
  noInterrupts(); 
  send_leader();

  for (i = 0; i < 13; ++i){
    send_byte(bytes[i]);
  }

  send_trailer();
  interrupts();
}

