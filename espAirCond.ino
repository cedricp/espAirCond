/*
   AirConditioner controller
   2016 Cedric PAILLE
   License : WTFPL (http://www.wtfpl.net/)
*/

#include "airton_control.h"
#include "fujitsu_control.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "YourSSIDHere";
const char *password = "YourPSKHere";

#define AIRCOND_GPIO_PIN 0

MDNSResponder mdns;
ESP8266WebServer server ( 80 );

fujitsu_contol fujitsu(AIRCOND_GPIO_PIN);
airton_control airton(AIRCOND_GPIO_PIN);

aircond_control* current_controller = NULL;

float get_temperature()
{
  return -1.;  
}

void handleNotFound() {
  String message = "File Not Found\n\n";
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
  bool retcode = false;

  current_controller = NULL;

  if ( server.hasArg("model") ) {
    if (server.arg("model") == "airton")
      current_controller = &airton;
    if (server.arg("model") == "fujitsu")
      current_controller = &fujitsu;
  }

  if (current_controller != NULL) {

    if ( server.args() == 0 ) {
      server.send(200, "text/plain", current_controller->get_as_json(get_temperature()));
    }

    if ( server.hasArg("temperature") ) {
      int temp = server.arg("temperature").toInt();
      retcode |= current_controller->set_temperature(temp);
    }

    if ( server.hasArg("mode") ) {
      String mode = server.arg("mode");
      if (mode == "auto")
        retcode |= current_controller->set_ac_mode(MODE_AUTO);
      else if (mode == "cool")
        retcode |= current_controller->set_ac_mode(MODE_COOL);
      else if (mode == "heat")
        retcode |= current_controller->set_ac_mode(MODE_HEAT);
      else if (mode == "fan")
        retcode |= current_controller->set_ac_mode(MODE_FAN);
      else if (mode == "dry")
        retcode |= current_controller->set_ac_mode(MODE_DRY);
      else
        server.send(200, "text/plain", "Argument 'mode' error");
      return;
    }

    if ( server.hasArg("fan") ) {
      String fan = server.arg("fan");
      if (fan == "auto")
        retcode |= current_controller->set_fan_mode(FAN_SPEED_AUTO);
      else if (fan == "low")
        retcode |= current_controller->set_fan_mode(FAN_SPEED_LOW);
      else if (fan == "mid")
        retcode |= current_controller->set_fan_mode(FAN_SPEED_MID);
      else if (fan == "high")
        retcode |= current_controller->set_fan_mode(FAN_SPEED_HIGH);
      else if (fan == "quiet")
        retcode |= current_controller->set_fan_mode(FAN_SPEED_QUIET);
      else
        server.send(200, "text/plain", "Argument 'fan' error");
      return;
    }
    if ( server.hasArg("power") ) {
      String power = server.arg("power");
      if (power == "on"){
        current_controller->poweron();
        server.send(200, "text/plain", "OK");
        return;
      }
      if (power == "off"){
        current_controller->poweroff();
        server.send(200, "text/plain", "OK");
        return;
      }      
    }
    current_controller->send_data();
    server.send(200, "text/plain", "OK");
  } else {
    String message = "You must define a valid aircond model\n\n";
    server.send(200, "text/plain", message);
  }

}

void setup() {
  WiFi.begin ( ssid, password );
  Serial.println ( "Warming up WIFI" );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }

  if ( mdns.begin ( "esp_aircond", WiFi.localIP() ) ) {

  }

  server.on ( "/control", handleRoot );
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  mdns.update();
  server.handleClient();
}

