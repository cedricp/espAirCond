#include "aircond_control.h"

aircond_control::aircond_control(int ir_pin, bool open_drain)
{
  ir.set_gpio_pin(ir_pin, open_drain);
  m_temperature  = 20;
  m_air_mode     = MODE_HEAT;
  m_fan_mode     = FAN_SPEED_AUTO;
  m_power_status = 0;
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
aircond_control::get_as_json(float curr_temp, float curr_humidity, const char* status)
{
  String json = "{\n";
  json += "\"ac_temperature\" : " + String(m_temperature) + "\n";
  json += "\"ac_power_state\" :" + power_to_string(m_power_status) + "\n";
  json += "\"ac_fan_mode\" :" + fan_to_string(m_fan_mode) + "\n";
  json += "\"ac_mode\":" + ac_to_string(m_air_mode) + "\n";
  json += "\"temperature\" :" + String(curr_temp) + "\n";
  json += "\"humidity\" : " + String(curr_humidity) + "\n";
  json += "\"DHTstatus\" : " + String(status) + "\n";
  json += "}\n";
  
  return json;
}

