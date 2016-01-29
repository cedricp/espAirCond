#include "aircond_control.h"

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
aircond_control::get_as_json(float curr_temp)
{
  String json = "{\n";
  json += "\"temperature\": " + String(m_temperature) + "\n";
  json += "\"power_state\":" + power_to_string(m_power_status) + "\n";
  json += "\"current_temp\":" + String(curr_temp) + "\n";
  json += "\"fan_mode\":" + fan_to_string(m_fan_mode) + "\n";
  json += "\"ac_mode\":" + ac_to_string(m_air_mode) + "\n";
  json += "}\n";
  
  return json;
}

