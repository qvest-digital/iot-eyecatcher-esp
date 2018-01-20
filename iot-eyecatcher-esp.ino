#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#include "user_interface.h"

#include "config.h"
#include "secrets.h"
#include "tools.h"
#include "temperature.h"

Temperature temperatureSensor(2); // Use D2 for OneWire bus

boolean temperatureEnabled = false;
float temperature;

void setup() {

  system_update_cpu_freq(160);
  
  Serial.begin(115200); 
  //Serial.setDebugOutput(true);
    
  wifiConnect();

  if (!temperatureSensor.init()) {
    Serial.println("ERROR: temperature sensor failure");
  } else {
    Serial.println("INFO:  temperature sensor connected");
    temperatureEnabled = true;
    temperatureSensor.request();
    temperature = temperatureSensor.read();
    Serial.print("INFO:  temperature readout is ");
    Serial.println(temperature);    
  }
    
  sendData();    

  Serial.print("\nINFO:  Falling asleep for ");
  Serial.print(DEEP_SLEEP / 1000000);
  Serial.println(" seconds.");

  ESP.deepSleep(DEEP_SLEEP);
  delay(100);  
}

void loop() {
  // nothing to do here
}

void sendData() {  
   WiFiClientSecure client;

  if (!client.connect(SERVER_HOST, 443)) {
    Serial.println("ERROR: connection failed");
    return;
  }   
 
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& root = jsonBuffer.createObject();
  root["name"] = NAME;
  
  if (temperatureEnabled) {
    root["temp"] = temperature;
  }
       
  client.printf("POST /%s/%s HTTP/1.1\n", SERVER_BROKER_PATH, TOPIC);
  client.printf("Host: %s\n", SERVER_HOST);
  client.println("Cache-Control: no-cache");
  client.println(HTTP_AUTH_HEADER);
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.printf("Content-Length: %d\n\n", root.measureLength());
  
  root.printTo(client);
               
  delay(100);
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println("\nINFO:  Connection closed");
}

void wifiConnect() {
  int retries = MAX_WIFI_RETRIES;
  Serial.println("");
  Serial.print("INFO:  Connecting to WiFi access point:  ");
  Serial.print(AP_SSID);
  WiFi.begin(AP_SSID, AP_PASSWORD);
  while ((WiFi.status() != WL_CONNECTED) && (retries--)) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.println("ERROR: WiFi connection not possible, will retry later");        
    ESP.deepSleep(DEEP_SLEEP_WIFI_FAILURE);
    delay(100);  
  }
  
  Serial.println("");
  Serial.println("INFO:  WiFi connected");  

}
