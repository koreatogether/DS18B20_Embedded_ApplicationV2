#pragma once
#include <stdint.h>

class ITemperatureSensor
{
public:
    virtual ~ITemperatureSensor() = default;
    virtual bool begin() = 0;
    virtual int getSensorCount() = 0;
    virtual bool getAddress(uint8_t index, uint64_t &address) = 0;
    virtual float getTemperature(uint8_t index) = 0;
};
