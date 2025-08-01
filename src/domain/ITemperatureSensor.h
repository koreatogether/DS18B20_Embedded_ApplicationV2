#pragma once
class ITemperatureSensor
{
public:
    virtual ~ITemperatureSensor() = default;
    virtual void begin() = 0;
    virtual float readTemperature() = 0;
};
