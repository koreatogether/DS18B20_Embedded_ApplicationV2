#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 데이터 핀 정의
#define ONE_WIRE_BUS 2

// OneWire 및 DallasTemperature 객체 생성
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ;
    } // 시리얼 연결 대기 (필요시)
    Serial.println("[시리얼 통신 시작: 115200 baud]");

    sensors.begin();
    Serial.println("[DallasTemperature 라이브러리 초기화 완료]");
}

void loop()
{
    // 추후 메뉴/센서 로직 구현 예정
}
