#pragma once
#include <Arduino.h>
#include <DallasTemperature.h>
#include "../domain/ITemperatureSensor.h"
#include "../domain/SensorStatus.h"

constexpr int SENSOR_MAX_COUNT = 8;

struct SensorRowInfo
{
    int idx;
    int logicalId;
    DeviceAddress addr;
    float temp;
    bool connected;
};

class SensorController
{
public:
    SensorController();

    // 센서 논리 ID 관리
    uint8_t getSensorLogicalId(int idx);
    void setSensorLogicalId(int idx, uint8_t newId);
    bool isIdDuplicated(int newId, int exceptIdx = -1);
    void assignIDsByAddress();

    // 센서 상태 테이블 관리
    void printSensorStatusTable();
    void updateSensorRows();
    const SensorRowInfo *getSortedSensorRows() const { return g_sortedSensorRows; }

    // 임계값 관리
    const char *getUpperState(float temp);
    const char *getLowerState(float temp);
    const char *getSensorStatus(float temp);

private:
    static SensorRowInfo g_sortedSensorRows[SENSOR_MAX_COUNT];
    void printSensorAddress(const DeviceAddress &addr);
    void printSensorRow(int idx, int id, const DeviceAddress &addr, float temp);
};
