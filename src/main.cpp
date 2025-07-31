#include <Arduino.h>

#include "infrastructure/DS18B20Sensor.h"
#include "domain/TemperatureSensorManager.h"
#include "application/TemperatureService.h"

DS18B20Sensor ds18b20(2);
TemperatureSensorManager sensorManager(&ds18b20);
TemperatureService tempService(&sensorManager);

unsigned long lastPrint = 0;
const unsigned long printInterval = 15000;
unsigned long lastBusInit = 0;
const unsigned long busInitInterval = 15000;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }
    Serial.println("[시리얼 통신 시작: 115200 baud]");
    ds18b20.begin();
    Serial.println("[DS18B20Sensor 초기화 완료]");
}

void loop()
{
    unsigned long now = millis();
    // 센서 연결/끊김에 대응: 주기적으로 버스 재초기화
    if (now - lastBusInit >= busInitInterval)
    {
        ds18b20.begin();
        lastBusInit = now;
    }
    if (now - lastPrint >= printInterval)
    {
        tempService.update();
        tempService.printStatus();
        lastPrint = now;
    }
    // 추후 메뉴/센서 로직 구현 예정
}
