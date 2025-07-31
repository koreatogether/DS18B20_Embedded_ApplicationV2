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

// 센서 더미 데이터 구조체
struct SensorStatus
{
    uint8_t id;
    uint64_t address;
    float temperature;
    float upperThreshold;
    const char *upperState;
    float lowerThreshold;
    const char *lowerState;
    const char *status;
};

SensorStatus sensorsTable[8]; // 실제 센서 정보로 갱신
int sensorCount = 0;

unsigned long lastPrint = 0;
const unsigned long printInterval = 15000; // 15초
unsigned long lastBusInit = 0;
const unsigned long busInitInterval = 15000; // 센서 버스 재초기화 주기(15초)

void printSensorTable()
{
    Serial.println();
    Serial.println("번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태");
    Serial.println("---- | --- | -------------- | --------- | -------- | -------- | -------- | -------- | --------");
    for (int i = 0; i < sensorCount; ++i)
    {
        char line[128];
        snprintf(line, sizeof(line),
                 "%2d   | %2d  | 0x%010llX | %7.1f°C | %7.1f°C | %-6s   | %7.1f°C | %-6s   | %-6s",
                 i + 1,
                 sensorsTable[i].id,
                 sensorsTable[i].address,
                 sensorsTable[i].temperature,
                 sensorsTable[i].upperThreshold,
                 sensorsTable[i].upperState,
                 sensorsTable[i].lowerThreshold,
                 sensorsTable[i].lowerState,
                 sensorsTable[i].status);
        Serial.println(line);
    }
    Serial.println();
}

void updateSensorsTable()
{
    sensorCount = sensors.getDeviceCount();
    if (sensorCount > 8)
        sensorCount = 8;
    DeviceAddress addr;
    sensors.requestTemperatures();
    for (int i = 0; i < sensorCount; ++i)
    {
        if (sensors.getAddress(addr, i))
        {
            uint64_t address = 0;
            for (int j = 0; j < 8; ++j)
            {
                address <<= 8;
                address |= addr[j];
            }
            float temp = sensors.getTempC(addr);
            sensorsTable[i].id = i + 1;
            sensorsTable[i].address = address;
            sensorsTable[i].temperature = temp;
            sensorsTable[i].upperThreshold = 30.0;
            sensorsTable[i].upperState = (temp > 30.0) ? "초과" : "정상";
            sensorsTable[i].lowerThreshold = 20.0;
            sensorsTable[i].lowerState = (temp < 20.0) ? "초과" : "정상";
            if (temp > 30.0 || temp < 20.0)
            {
                sensorsTable[i].status = "경고";
            }
            else
            {
                sensorsTable[i].status = "정상";
            }
        }
    }
}

void loop()
{
    unsigned long now = millis();

    // 센서 연결/끊김에 대응: 주기적으로 버스 재초기화
    if (now - lastBusInit >= busInitInterval)
    {
        sensors.begin();
        lastBusInit = now;
    }

    if (now - lastPrint >= printInterval)
    {
        updateSensorsTable();
        printSensorTable();
        lastPrint = now;
    }
    // 추후 메뉴/센서 로직 구현 예정
}
