#pragma once
#include "../domain/ITemperatureSensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor : public ITemperatureSensor
{
public:
    DS18B20Sensor(uint8_t pin);
    bool begin() override;
    int getSensorCount() override;
    bool getAddress(uint8_t index, uint64_t &address) override;
    float getTemperature(uint8_t index) override;

private:
    uint8_t pin;
    OneWire oneWire;
    DallasTemperature sensors;
};
