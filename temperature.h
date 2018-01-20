#pragma once

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define RESOLUTION 10
#define DELAY_IN_MS (750 / (1 << (12 - RESOLUTION)))
    
class Temperature {

  private:
    OneWire oneWire;
    DallasTemperature sensors;
    byte address[8];
    boolean searchTemperatureSensor();

  public:
    Temperature(uint8_t i2cAddress);
    boolean init();
    void request();
    float read();
};
