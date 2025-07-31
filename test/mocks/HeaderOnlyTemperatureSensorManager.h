#pragma once
#include "MockTemperatureSensor.h"
#include "../../src/domain/SensorStatus.h"
#include <cstddef>

class HeaderOnlyTemperatureSensorManager
{
public:
    HeaderOnlyTemperatureSensorManager(MockTemperatureSensor *sensor)
        : sensor(sensor), sensorCount(0) {}

    void update()
    {
        if (!sensor)
            return;
        sensorCount = sensor->getSensorCount();
        if (sensorCount > 8)
            sensorCount = 8;
        for (int i = 0; i < sensorCount; ++i)
        {
            uint64_t address = 0;
            if (sensor->getAddress(i, address))
            {
                sensorsTable[i].id = i + 1;
                sensorsTable[i].address = address;
                sensorsTable[i].temperature = sensor->getTemperature(i);
                sensorsTable[i].upperThreshold = 30.0f;
                sensorsTable[i].upperState = sensorsTable[i].temperature > 30.0f ? "초과" : "정상";
                sensorsTable[i].lowerThreshold = 20.0f;
                sensorsTable[i].lowerState = sensorsTable[i].temperature < 20.0f ? "초과" : "정상";
                if (sensorsTable[i].temperature > 30.0f)
                    sensorsTable[i].status = "경고";
                else if (sensorsTable[i].temperature < 20.0f)
                    sensorsTable[i].status = "경고";
                else
                    sensorsTable[i].status = "정상";
            }
        }
    }

    int getSensorCount() const { return sensorCount; }
    const SensorStatus *getSensorStatus(int index) const
    {
        if (index < 0 || index >= sensorCount)
            return nullptr;
        return &sensorsTable[index];
    }

private:
    MockTemperatureSensor *sensor;
    SensorStatus sensorsTable[8];
    int sensorCount;
};
