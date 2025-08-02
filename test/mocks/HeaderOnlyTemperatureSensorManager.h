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
            
        initializeSensorCount();
        updateAllSensors();
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
    
    // Helper methods for update
    void initializeSensorCount()
    {
        sensorCount = sensor->getSensorCount();
        if (sensorCount > 8)
            sensorCount = 8;
    }
    
    void updateAllSensors()
    {
        for (int i = 0; i < sensorCount; ++i)
        {
            updateSingleSensor(i);
        }
    }
    
    void updateSingleSensor(int i)
    {
        uint64_t address = 0;
        if (sensor->getAddress(i, address))
        {
            populateSensorData(i, address);
            calculateThresholdStates(i);
            determineSensorStatus(i);
        }
    }
    
    void populateSensorData(int i, uint64_t address)
    {
        sensorsTable[i].id = i + 1;
        sensorsTable[i].address = address;
        sensorsTable[i].temperature = sensor->getTemperature(i);
        sensorsTable[i].upperThreshold = 30.0f;
        sensorsTable[i].lowerThreshold = 20.0f;
    }
    
    void calculateThresholdStates(int i)
    {
        sensorsTable[i].upperState = sensorsTable[i].temperature > 30.0f ? "초과" : "정상";
        sensorsTable[i].lowerState = sensorsTable[i].temperature < 20.0f ? "초과" : "정상";
    }
    
    void determineSensorStatus(int i)
    {
        if (sensorsTable[i].temperature > 30.0f)
            sensorsTable[i].status = "경고";
        else if (sensorsTable[i].temperature < 20.0f)
            sensorsTable[i].status = "경고";
        else
            sensorsTable[i].status = "정상";
    }
};
