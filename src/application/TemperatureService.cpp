#include "TemperatureService.h"
#include <Arduino.h>

TemperatureService::TemperatureService(TemperatureSensorManager *manager)
    : manager(manager) {}

void TemperatureService::update()
{
    manager->update();
}

void TemperatureService::printStatus()
{
    Serial.println();
    Serial.println("번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태");
    Serial.println("---- | --- | -------------- | --------- | -------- | -------- | -------- | -------- | --------");
    int count = manager->getSensorCount();
    for (int i = 0; i < count; ++i)
    {
        const SensorStatus *status = manager->getSensorStatus(i);
        if (!status)
            continue;
        char line[128];
        snprintf(line, sizeof(line),
                 "%2d   | %2d  | 0x%010llX | %7.1f°C | %7.1f°C | %-6s   | %7.1f°C | %-6s   | %-6s",
                 i + 1,
                 status->id,
                 status->address,
                 status->temperature,
                 status->upperThreshold,
                 status->upperState,
                 status->lowerThreshold,
                 status->lowerState,
                 status->status);
        Serial.println(line);
    }
    Serial.println();
}
