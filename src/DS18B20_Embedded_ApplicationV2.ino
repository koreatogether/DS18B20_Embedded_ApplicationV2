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
    // 추가 안전장치: setup 완료 후 1초 대기하여 시리얼 통신 안정화
    delay(1000);
    
    // 강제로 Normal 상태 확인 및 설정
    menuController.resetToNormalState();
    
    Serial.println("=== 시스템 초기화 완료 ===");
    Serial.print("현재 AppState: ");
    Serial.println((int)menuController.getAppState());
    Serial.println("센서 제어 메뉴 진입: 'menu' 또는 'm' 입력");
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
    
    // 명시적으로 Normal 상태로 초기화 및 상태 출력
    menuController.setAppState(AppState::Normal);
    Serial.println("[시스템 상태: Normal 모드로 초기화 완료]");
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
