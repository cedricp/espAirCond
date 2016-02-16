/*
 * Airton InfraRed module
 * For YKRH remote control replacement
 * Cedric PAILLE 2016
 * 
 * Airton IR code is composed of 13 bytes
 * the IR frequency is 38Khz (33% duty cycle)
 * Bytes are sent in LSB first order
 * Byte 0 is a constant value (I guess the "address" of the aircond)
 * Bytes 2,5,7,8,10 are not used
 * Byte 1 is the temperature in celcius - 8 with a logical shift left of 3 [ttttt***]
 * Byte 12 is the crc code (sum of bytes 0-11 & 0xFF)
 * Byte 9 is the power bit [******p*] (p = 0 -> off, p = 1 ->on)
 * 
 * Leader frame :
 * 8100 us IR on + 4400 IR off
 * 
 * Trailer frame:
 * 650us IR on + 5000 IR off
 * 
 * Bit zero :
 * 650us IR on + 450 us IR off (1100 us total)
 * 
 * Bit one :
 * 650us IR on + 1550 IR off (2200us total)
 * 
 */

  
#include "airton_control.h"

#define AIRTON_MODE_HEAT 0x80
#define AIRTON_MODE_COOL 0x30
#define AIRTON_MODE_DRY  0x40
#define AIRTON_MODE_AUTO 0x10
#define AIRTON_MODE_FAN  0x70

#define AIRTON_FAN_SPEED_AUTO 0b00000101
#define AIRTON_FAN_SPEED_HIGH 0b00000001
#define AIRTON_FAN_SPEED_LOW  0b00000011
#define AIRTON_FAN_SPEED_MID  0b00000010

#define AIRTON_SWING_OFF 0b00000111
#define AIRTON_SWING_ON  0x00

#define POWER_ON  0x20
#define POWER_OFF 0x00 

airton_control::airton_control(int ir_pin) : aircond_control(ir_pin)
{
  ir.set_period(38);
  
  m_temperature   = 19;
  m_power_status  = POWER_ON;
  m_air_mode      = AIRTON_MODE_HEAT;
  m_fan_mode      = AIRTON_FAN_SPEED_AUTO;
  m_swing_v       = AIRTON_SWING_ON;
}

airton_control::~airton_control()
{
  
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
  int fan_mode, air_mode, power_status, temp, swing;
  // No "quiet mode" on Airton
  switch(m_fan_mode){
  case FAN_SPEED_LOW:
  case FAN_SPEED_QUIET:
    fan_mode = AIRTON_FAN_SPEED_LOW;
    break;
  case FAN_SPEED_MID:
    fan_mode = AIRTON_FAN_SPEED_MID;
    break;
  case FAN_SPEED_HIGH:
    fan_mode = AIRTON_FAN_SPEED_HIGH;
    break;
  case FAN_SPEED_AUTO:
  default:
    fan_mode = AIRTON_FAN_SPEED_AUTO;
    break;
  }

  switch(m_air_mode){
    case MODE_COOL:
      air_mode = AIRTON_MODE_COOL;
      break;
    case MODE_HEAT:
      air_mode = AIRTON_MODE_HEAT;
      break;
    case MODE_DRY:
      air_mode = AIRTON_MODE_DRY;
      break;
    case MODE_FAN:
      air_mode = AIRTON_MODE_FAN;
      break;
    case MODE_AUTO:
    default:
      air_mode = AIRTON_MODE_AUTO;
      break;
  }

  swing =  m_swing_v ? AIRTON_SWING_ON : AIRTON_SWING_OFF;
  power_status = m_power_status ? POWER_ON : POWER_OFF;

  temp = m_temperature > 32 ? 32 : temp;
  temp = temp < 16 ? 16 : temp;
  
  static char bytes[13], i;
  bytes[0]  = 0xC3;
  bytes[1]  = ((temp - 8) << 3) | swing;
  bytes[2]  = 0x00;
  bytes[3]  = 0x00;
  bytes[4]  = fan_mode << 5;
  bytes[5]  = 0x00;
  bytes[6]  = air_mode;
  bytes[7]  = 0x00;
  bytes[8]  = 0x00;
  bytes[9]  = power_status;
  bytes[10] = 0x00;
  bytes[11] = 0x01;
  bytes[12] = compute_crc(bytes);

  while(!ir.can_begin_send()){
    // Don't start until we risk a clock overflow
  }
  
  noInterrupts(); 
  send_leader();

  for (i = 0; i < 13; ++i){
    send_byte(bytes[i]);
  }

  send_trailer();
  interrupts();
}

