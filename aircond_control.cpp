#include "aircond_control.h"
#include <EEPROM.h>

aircond_control::aircond_control(int ir_pin, bool open_drain)
{
  ir.set_gpio_pin(ir_pin, open_drain);
  init();
}

void
aircond_control::init()
{
  m_temperature  = 20;
  m_air_mode     = MODE_HEAT;
  m_fan_mode     = FAN_SPEED_AUTO;
  m_power_status = 0;
  m_swing_v      = false;
  m_swing_h      = false;
}

String ac_to_string(char airmode){
  switch(airmode){
    case MODE_COOL:
      return "COOL";
    case MODE_HEAT:
      return "HEAT";
    case MODE_DRY:
      return "DRY";
    case MODE_FAN:
      return "FAN";
    case MODE_AUTO:
      return "AUTO";
    default:
      return "UNKNOWN";
  }
  return "ooops";
}

String power_to_string(char power){
  switch(power){
    case 0:
      return "OFF";
    case 1:
    default:
      return "ON";
  }
  return "ooops";
}

String fan_to_string(char fanmode){
  switch (fanmode){
    case FAN_SPEED_AUTO:
      return "AUTO";
    case FAN_SPEED_LOW:
      return "LOW";
    case FAN_SPEED_HIGH:
      return "HIGH";
    case FAN_SPEED_MID:
      return "MEDIUM";
    case FAN_SPEED_QUIET:
      return "QUIET";
    default:
      return "UNKNOWN";
  }
  return "ooops";
}

String
aircond_control::get_as_json(float curr_temp, float curr_humidity, const char* status, unsigned reconnect)
{
  String json = "{\n";
  json += "\"ac_temperature\" : " + String((int)m_temperature) + "\n";
  json += "\"ac_power_state\" : '" + power_to_string(m_power_status) + "'\n";
  json += "\"ac_fan_mode\" : '" + fan_to_string(m_fan_mode) + "'\n";
  json += "\"ac_mode\" : '" + ac_to_string(m_air_mode) + "'\n";
  json += "\"temperature\" : " + String(curr_temp) + "\n";
  json += "\"humidity\" : " + String(curr_humidity) + "\n";
  json += "\"DHTstatus\" : '" + String(status) + "'\n";
  json += "\"TimeSinceBoot\" : " + String(millis()/1000) + "\n";
  json += "\"WifiReconnect\" : " + String(reconnect) + "\n";
  json += "}\n";
  
  return json;
}

bool aircond_control::restore_from_eeprom()
{
  unsigned start = m_id * 8;
  char* data = &m_temperature;
  char crc = 0, crccheck = 0;
  int i;
  for (i = 0; i < 7; ++i){
    *data = char(EEPROM.read(i+start));
    crc += *data;
    data++;
  }
  
  crccheck = char(EEPROM.read(i+start));
  
  if (crccheck != crc)
    return false;
  return true;
}

void aircond_control::save_to_eeprom()
{
  unsigned start = m_id * 8;
  char* data = &m_temperature;
  char crc = 0x00;
  int i;
  for (i = 0; i < 7; ++i){
    EEPROM.write(i+start, *data);
    crc += *data;
    data++;
  }
  EEPROM.write(i+start, crc);

  EEPROM.commit();
}

