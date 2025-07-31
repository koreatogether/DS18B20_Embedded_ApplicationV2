#pragma once
#include "../domain/TemperatureSensorManager.h"
#include "../domain/ITemperatureSensor.h"

class TemperatureService
{
public:
    TemperatureService(TemperatureSensorManager *manager);
    void update();
    void printStatus();

private:
    TemperatureSensorManager *manager;
};
