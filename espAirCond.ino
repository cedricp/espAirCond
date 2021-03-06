/*
   AirConditioner controller
   2016 Cedric PAILLE
   License : WTFPL (http://www.wtfpl.net/)
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include "airton_control.h"
#include "fujitsu_control.h"
#include "dht.h"
#include "config.h"

/*
 * define these constants in config.h
 * prog_char ssid[]      PROGMEM = "your_ssid";
 * prog_char password[]  PROGMEM = "your_password";
 */

#define WITH_DHT 1

// GPIO Config
#define AIRCOND_GPIO_LED_PIN 3
#define AIRCOND_GPIO_LED_OPEN_DRAIN false
#define DHT22_PIN 2
// Are we using DHT module ?

#if WITH_DHT == 1
DHT dht;
#endif

// Globals
fujitsu_control  fujitsu(AIRCOND_GPIO_LED_PIN, AIRCOND_GPIO_LED_OPEN_DRAIN);
airton_control   airton(AIRCOND_GPIO_LED_PIN, AIRCOND_GPIO_LED_OPEN_DRAIN);
MDNSResponder    mdns;
ESP8266WebServer server ( 80 );
aircond_control* current_controller = NULL;
unsigned reconnect_count;

void handleNotFound() {
  String message = "Invalid command, go away\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRoot() {
  current_controller = NULL;

  if ( server.hasArg("model") ) {
    if (server.arg("model") == "airton")
      current_controller = &airton;
    if (server.arg("model") == "fujitsu")
      current_controller = &fujitsu;
  }

  if ( current_controller != NULL ) {

    if ( server.args() == 1 ) {
#if WITH_DHT == 1
      float temp = dht.getTemperature();
      float hum  = dht.getHumidity();
      const char* sts = dht.getStatusString();
#else
      float temp = 0.;
      float hum  = 0.;
      const char* sts = "DHT module not installed";
#endif
      server.send(200, "text/plain", current_controller->get_as_json(temp, hum, sts, reconnect_count));
      return;
    }

    if ( server.hasArg("temperature") ) {
      int temp = server.arg("temperature").toInt();
      current_controller->set_temperature(temp);
    }

    if ( server.hasArg("mode") ) {
      String mode = server.arg("mode");
      if (mode == "auto")
        current_controller->set_ac_mode(MODE_AUTO);
      else if (mode == "cool")
        current_controller->set_ac_mode(MODE_COOL);
      else if (mode == "heat")
        current_controller->set_ac_mode(MODE_HEAT);
      else if (mode == "fan")
        current_controller->set_ac_mode(MODE_FAN);
      else if (mode == "dry")
        current_controller->set_ac_mode(MODE_DRY);
      else if (mode == "init"){
        current_controller->init();
        goto END;
      }
    }

    if ( server.hasArg("fan") ) {
      String fan = server.arg("fan");
      if (fan == "auto")
        current_controller->set_fan_mode(FAN_SPEED_AUTO);
      else if (fan == "low")
        current_controller->set_fan_mode(FAN_SPEED_LOW);
      else if (fan == "mid")
        current_controller->set_fan_mode(FAN_SPEED_MID);
      else if (fan == "high")
        current_controller->set_fan_mode(FAN_SPEED_HIGH);
      else if (fan == "quiet")
        current_controller->set_fan_mode(FAN_SPEED_QUIET);
    }

    if ( server.hasArg("swing") ) {
      String mode = server.arg("swing");
      if (mode == "h")
        current_controller->set_swing_h(true);
        current_controller->set_swing_v(false);
      if (mode == "v")
        current_controller->set_swing_v(true);
        current_controller->set_swing_h(false);
      if (mode == "hv")
        current_controller->set_swing_h(true);
        current_controller->set_swing_v(true);
      if (mode == "off")
        current_controller->set_swing_h(false);
        current_controller->set_swing_v(false);
    }
    
    if ( server.hasArg("power") ) {
      String power = server.arg("power");
      if (power == "on"){
        current_controller->poweron();
        goto END;
      }
      if (power == "off"){
        current_controller->poweroff();
        goto END;
      }      
    }

    if (current_controller->is_on())
      current_controller->send_data();
    
  } else {
    String message = "You must define a valid aircond model\n\n";
    server.send(200, "text/plain", message);
    return;
  }

END:
  current_controller->save_to_eeprom();
  server.send(200, "text/plain", "OK");
  return;
}

void setup_wifi()
{
  WiFi.begin ( ssid, password );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
  }
  
  mdns.begin ( "esp_ac", WiFi.localIP() );
}

void setup() {
  reconnect_count = 0;
  EEPROM.begin(512);
  
  setup_wifi();
  
  server.on ( "/control", handleRoot );
  server.onNotFound(handleNotFound);
  server.begin();

#if WITH_DHT == 1
  dht.setup(DHT22_PIN, DHT::DHT22);
#endif



  bool ok;
  ok = fujitsu.restore_from_eeprom();
  if (!ok) fujitsu.init();
  ok = airton.restore_from_eeprom();
  if (!ok) airton.init();
}

void loop() {
  if ( WiFi.status() != WL_CONNECTED ) {
    setup_wifi();
    reconnect_count++;
    return;
  }
    
  mdns.update();
  server.handleClient();
}

