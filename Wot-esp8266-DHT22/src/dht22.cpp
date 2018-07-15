//
// Created by simone on 15/07/18.
//
/**
 * Simple ESP8266 server compliant with Mozilla's proposed WoT API
 * Based on the HelloServer example
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include "DHTesp.h"

//TODO: Hardcode your wifi credentials here (and keep it private)
const char* ssid = "test-iot";
const char* password = "mozillaiot";
const int ledPin = LED_BUILTIN;


WebThingAdapter adapter("Tempeature and Humidity sensor");

ThingDevice tempHumSensor("temperature-humidity", "temperature and humidity sensor using DHT22", "multiSensor");

ThingProperty temp("temperature", "measured temperature", NUMBER);
ThingProperty hum("humidity", "measured humidity", NUMBER);
ThingPropertyValue readingValue;
DHTesp dht;
auto lastTime = millis();

void setup(void){

  Serial.begin(115200);
  delay(200);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(ledPin, HIGH); // active low led

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  tempHumSensor.addProperty(&temp);
  tempHumSensor.addProperty(&hum);
  adapter.addDevice(&tempHumSensor);
  readingValue.number = (double)dht.getTemperature();
  temp.setValue(readingValue);
  readingValue.number = (double)dht.getHumidity();
  hum.setValue(readingValue);
  adapter.begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(tempHumSensor.id);

  dht.setup(D7, DHTesp::DHT22);
}

void loop(void){
  adapter.update();
  if ( millis() - lastTime >= dht.getMinimumSamplingPeriod()){
    readingValue.number = (double)dht.getTemperature();
    temp.setValue(readingValue);
    readingValue.number = (double)dht.getHumidity();
    hum.setValue(readingValue);
    lastTime = millis();
  }

}

