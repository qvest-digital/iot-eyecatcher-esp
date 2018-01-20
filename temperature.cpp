#include "temperature.h"

Temperature::Temperature(uint8_t oneWireBus) : 
  oneWire(oneWireBus),
  sensors(&oneWire)
  {}

boolean Temperature::init() {
    sensors.begin();    
    sensors.setResolution(RESOLUTION);
    sensors.setWaitForConversion(true);
    return searchTemperatureSensor();  
}

void Temperature::request() {
    sensors.requestTemperatures();
}

float Temperature::read() {
    return sensors.getTempC(address);    
}

boolean Temperature::searchTemperatureSensor() {
  byte i;
  byte present = 0;
  byte data[12];

  oneWire.reset_search();
  if ( !oneWire.search(address)) {
      Serial.println("ERROR: No more addresses, no temperature sensor found.");
      oneWire.reset_search();
      return false;
  }

  Serial.print("INFO:  Found device ");
  for( i = 0; i < 8; i++) {
    Serial.print(address[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( address, 7) != address[7]) {
      Serial.println("ERROR: CRC is not valid!");    
      return false;
  }

  if ((address[0] == 0x10 ) || (address[0] == 0x28)) {
      Serial.println("\nINFO:  Device is a DS18S20 family device.");
      return true;
  }
  
  Serial.print("\nERROR: Device family is not recognized: 0x");
  Serial.println(address[0],HEX);
  return false;  
}

