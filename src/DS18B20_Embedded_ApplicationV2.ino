

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

constexpr uint8_t ONE_WIRE_BUS = 2;
constexpr float UPPER_THRESHOLD = 30.0;
constexpr float LOWER_THRESHOLD = 20.0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
unsigned long lastPrint = 0;
const unsigned long printInterval = 15000;

enum class AppState
{
    Normal,
    Menu
};
AppState appState = AppState::Normal;
bool firstLoop = true;
String inputBuffer;

// 함수 선언
void printSensorAddress(const DeviceAddress &addr);
const char *getUpperState(float temp);
const char *getLowerState(float temp);
const char *getSensorStatus(float temp);
void printSensorRow(int idx, int id, const DeviceAddress &addr, float temp);
void printSensorStatusTable();
void printMenu();
void handleSerialInput();
void setupSerialAndSensor();
void handleNormalState(unsigned long now);

// setup 함수
void setup()
{
    setupSerialAndSensor();
}

// loop 함수
void loop()
{
    handleSerialInput();
    unsigned long now = millis();
    if (appState == AppState::Normal)
    {
        handleNormalState(now);
    }
}

// 이하 모든 로직 함수 정의
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
    Serial.println("센서 제어 메뉴 진입: 'menu' 또는 'm' 입력");
    Serial.println("(센서 ID/임계값/상태 관리 등은 메뉴에서 설정 가능)");
}

void printMenu()
{
    Serial.println();
    Serial.println("===== 센서 제어 임시 메뉴 =====");
    Serial.println("1. 임시메뉴");
    Serial.println("2. 취소");
    Serial.print("메뉴 번호를 입력하세요: ");
}

void handleSerialInput()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\r' || c == '\n')
        {
            if (inputBuffer.length() > 0)
            {
                if (appState == AppState::Normal)
                {
                    if (inputBuffer == "menu" || inputBuffer == "m")
                    {
                        appState = AppState::Menu;
                        printMenu();
                    }
                }
                else if (appState == AppState::Menu)
                {
                    if (inputBuffer == "2")
                    {
                        appState = AppState::Normal;
                        printSensorStatusTable();
                        lastPrint = millis();
                    }
                    else
                    {
                        Serial.println("지원하지 않는 메뉴입니다. 2를 입력하면 취소됩니다.");
                        printMenu();
                    }
                }
                inputBuffer = "";
            }
        }
        else if (!isspace(c))
        {
            inputBuffer += c;
        }
    }
}
