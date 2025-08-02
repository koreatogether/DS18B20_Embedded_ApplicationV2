#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "application/SensorController.h"
#include "application/MenuController.h"

constexpr uint8_t ONE_WIRE_BUS = 2;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
unsigned long lastPrint = 0;
const unsigned long printInterval = 15000;

// 컨트롤러 인스턴스
SensorController sensorController;
MenuController menuController;
bool firstLoop = true;

void setup()
{
    setupSerialAndSensor();
}

void loop()
{
    menuController.handleSerialInput();
    unsigned long now = millis();
    if (menuController.getAppState() == AppState::Normal)
    {
        handleNormalState(now);
    }
}

void setupSerialAndSensor()
{
    Serial.begin(115200);
    Serial.println("[시리얼 통신 시작: 115200 baud]");
    Serial.print("Firmware build: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
    sensors.begin();
    Serial.println("[DS18B20Sensor 초기화 완료]");
}

void handleNormalState(unsigned long now)
{
    if (firstLoop)
    {
        sensorController.printSensorStatusTable();
        lastPrint = now;
        firstLoop = false;
    }
    else if (now - lastPrint >= printInterval)
    {
        sensorController.printSensorStatusTable();
        lastPrint = now;
    }
}
