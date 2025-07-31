#pragma once
#include "ITemperatureSensor.h"
#include "SensorStatus.h"

class TemperatureSensorManager
{
public:
    TemperatureSensorManager(ITemperatureSensor *sensor);
    void update();
    int getSensorCount() const;
    const SensorStatus *getSensorStatus(int index) const;

private:
    ITemperatureSensor *sensor;
    SensorStatus sensorsTable[8];
    int sensorCount;
};
