#pragma once
#include <Arduino.h>
#include <EEPROM.h>

constexpr int SENSOR_MAX_COUNT = 8;
constexpr int SENSOR_ID_EEPROM_ADDR = 0; // 시작 주소

// 센서 논리 ID를 EEPROM에서 읽어옴
inline void loadSensorIds(int *ids)
{
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        ids[i] = EEPROM.read(SENSOR_ID_EEPROM_ADDR + i);
        if (ids[i] < 1 || ids[i] > SENSOR_MAX_COUNT)
            ids[i] = i + 1; // 유효하지 않으면 기본값
    }
}

// 센서 논리 ID를 EEPROM에 저장
inline void saveSensorId(int idx, int id)
{
    EEPROM.write(SENSOR_ID_EEPROM_ADDR + idx, id);
    EEPROM.commit();
}
