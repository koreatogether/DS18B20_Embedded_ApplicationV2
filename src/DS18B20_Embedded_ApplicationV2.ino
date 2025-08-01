
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

constexpr uint8_t ONE_WIRE_BUS = 2;
constexpr float UPPER_THRESHOLD = 30.0;
constexpr float LOWER_THRESHOLD = 20.0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
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

unsigned long lastPrint = 0;
const unsigned long printInterval = 15000;

void printSensorAddress(const DeviceAddress &addr)
{
    Serial.print("0x");
    for (uint8_t j = 0; j < 8; j++)
    {
        if (addr[j] < 16)
            Serial.print("0");
        Serial.print(addr[j], HEX);
    }
}

const char *getUpperState(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "-";
    return (temp > UPPER_THRESHOLD) ? "초과" : "정상";
}

const char *getLowerState(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "-";
    return (temp < LOWER_THRESHOLD) ? "초과" : "정상";
}

const char *getSensorStatus(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "오류";
    return "정상";
}

void printSensorRow(int idx, int id, const DeviceAddress &addr, float temp)
{
    Serial.print("| ");
    Serial.print(idx + 1);
    Serial.print("    | ");
    Serial.print(id);
    Serial.print("   | ");
    printSensorAddress(addr);
    Serial.print(" | ");
    if (temp == DEVICE_DISCONNECTED_C)
        Serial.print("N/A   ");
    else
    {
        Serial.print(temp, 1);
        Serial.print("°C   ");
    }
    Serial.print(" | ");
    Serial.print(UPPER_THRESHOLD, 1);
    Serial.print("°C       | ");
    Serial.print(getUpperState(temp));
    Serial.print("         | ");
    Serial.print(LOWER_THRESHOLD, 1);
    Serial.print("°C       | ");
    Serial.print(getLowerState(temp));
    Serial.print("         | ");
    Serial.print(getSensorStatus(temp));
    Serial.println("     |");
}

void printSensorStatusTable()
{
    Serial.println("| 번호 | ID  | 센서 주소           | 현재 온도 | 상한설정온도 | 상한초과상태 | 하한설정온도 | 하한초과상태 | 센서상태 |");
    Serial.println("| ---- | --- | ------------       | ---------  | ------------  | ------------ | ------------ | ------------ | -------- |");

    sensors.requestTemperatures();
    DeviceAddress addr;
    int deviceCount = sensors.getDeviceCount();
    for (int i = 0; i < deviceCount; ++i)
    {
        if (sensors.getAddress(addr, i))
        {
            float temp = sensors.getTempC(addr);
            printSensorRow(i, i + 1, addr, temp);
        }
    }
    Serial.println("=================================================================================================================");
    Serial.println("=================================================================================================================");
}

bool firstLoop = true;
void loop()
{
    unsigned long now = millis();
    if (firstLoop)
    {
        printSensorStatusTable();
        lastPrint = now;
        firstLoop = false;
    }
    else if (now - lastPrint >= printInterval)
    {
        printSensorStatusTable();
        lastPrint = now;
    }
}