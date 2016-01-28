#include "aircond_control.h"

String
aircond_control::get_as_json(float curr_temp)
{
  String json = "{\n";
  json += "\"temperature\": " + String(m_temperature) + "\n";
  json += "\"power_state\":" + String(m_power_status) + "\n";
  json += "\"current_temp\":" + String(curr_temp) + "\n";
  json += "}\n";
  
  return json;
}

