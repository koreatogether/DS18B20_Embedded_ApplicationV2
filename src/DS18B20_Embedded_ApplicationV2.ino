#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <algorithm>

constexpr uint8_t ONE_WIRE_BUS = 2;
constexpr float UPPER_THRESHOLD = 30.0;
constexpr float LOWER_THRESHOLD = 20.0;
constexpr int SENSOR_MAX_COUNT = 8;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
unsigned long lastPrint = 0;
const unsigned long printInterval = 15000;

int selectedSensorIdx = -1;
int selectedDisplayIdx = -1; // 메뉴에서 입력한 번호 (1~8)
int inputNewId = -1;

// DS18B20 센서 내장 EEPROM의 alarmHigh/alarmLow(2바이트)를 논리 ID로 사용 (UserData API 활용)
uint8_t getSensorLogicalId(int idx)
{
    int id = sensors.getUserDataByIndex(idx);
    // Serial.print("[DEBUG] getSensorLogicalId idx:");
    // Serial.print(idx);
    // Serial.print(" userData:");
    // Serial.print(id);
    if (id < 1 || id > SENSOR_MAX_COUNT)
    {
        // Serial.print(" (유효X, 기본값 반환:");
        // Serial.print(idx + 1);
        // Serial.println(")");
        return idx + 1;
    }
    // Serial.print(" (정상 반환)");
    // Serial.println();
    return id;
}

void setSensorLogicalId(int idx, uint8_t newId)
{
    sensors.setUserDataByIndex(idx, newId);
    delay(30); // EEPROM write 여유 대기
    int verify = sensors.getUserDataByIndex(idx);
    Serial.print("[진단] setSensorLogicalId idx:");
    Serial.print(idx);
    Serial.print(" userData(변경후):");
    Serial.print(verify);
    Serial.print(" (기대값:");
    Serial.print(newId);
    Serial.println(")");
}

// 논리 ID 중복 체크 함수 (연결된 센서만 검사)
bool isIdDuplicated(int newId, int exceptIdx = -1)
{
    int deviceCount = sensors.getDeviceCount();
    for (int i = 0; i < deviceCount; ++i)
    {
        if (i == exceptIdx)
            continue;
        if (getSensorLogicalId(i) == newId)
            return true;
    }
    return false;
}

// 주소순 자동 ID 할당: 하드웨어 주소 기준으로 센서 정렬 후 논리 ID 1..N 부여
void assignIDsByAddress()
{
    int count = sensors.getDeviceCount();
    std::vector<int> idxs;
    idxs.reserve(count);
    for (int i = 0; i < count; ++i)
        idxs.push_back(i);
    std::sort(idxs.begin(), idxs.end(), [&](int a, int b)
              {
        DeviceAddress addrA, addrB;
        sensors.getAddress(addrA, a);
        sensors.getAddress(addrB, b);
        return memcmp(addrA, addrB, sizeof(DeviceAddress)) < 0; });
    for (int j = 0; j < (int)idxs.size(); ++j)
    {
        setSensorLogicalId(idxs[j], j + 1);
    }
}

enum class AppState
{
    Normal,
    Menu,
    SensorIdMenu,
    SensorIdChange_SelectSensor,  // 개별 센서 ID 변경: 센서 선택
    SensorIdChange_ConfirmSensor, // 센서 확인
    SensorIdChange_InputId,       // ID 입력
    SensorIdChange_ConfirmId,     // ID 확인
    SensorIdChange_Apply,         // ID 적용
};
AppState appState = AppState::Normal;
bool firstLoop = true;
String inputBuffer;

// 전역 센서 정렬 정보 저장
struct SensorRowInfo
{
    int idx;
    int logicalId;
    DeviceAddress addr;
    float temp;
    bool connected;
};
static SensorRowInfo g_sortedSensorRows[SENSOR_MAX_COUNT];

// 함수 선언
void printSensorAddress(const DeviceAddress &addr);
const char *getUpperState(float temp);
const char *getLowerState(float temp);
const char *getSensorStatus(float temp);
void printSensorRow(int idx, int id, const DeviceAddress &addr, float temp);
void printSensorStatusTable();
void printMenu();
void printSensorIdMenu();
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
    // DS18B20 센서 내장 EEPROM에서 논리 ID를 읽으므로 별도 MCU EEPROM 사용 안함
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
    Serial.print(id);
    Serial.print("    | ");
    uint8_t logicalId = getSensorLogicalId(idx);
    bool idValid = (logicalId >= 1 && logicalId <= SENSOR_MAX_COUNT);
    if (id == -1)
    {
        Serial.print("NONE   | NONE         | N/A     | N/A       | N/A         | N/A         | N/A       | N/A         | N/A     |");
    }
    else
    {
        if (idValid)
        {
            Serial.print(logicalId);
            Serial.print("   | ");
        }
        else
        {
            Serial.print("ERR");
            Serial.print("   | ");
        }
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
        Serial.print("     |");
    }
    Serial.println();
}

void printSensorStatusTable()
{
    Serial.println("| 번호 | ID  | 센서 주소           | 현재 온도 | 상한설정온도 | 상한초과상태 | 하한설정온도 | 하한초과상태 | 센서상태 |");
    Serial.println("| ---- | --- | ------------       | ---------  | ------------  | ------------ | ------------ | ------------ | -------- |");
    sensors.requestTemperatures();
    int deviceCount = sensors.getDeviceCount();
    bool idErrorFound = false;
    String idErrorList = "";
    std::vector<SensorRowInfo> sensorRows;
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        DeviceAddress addr = {0};
        float temp = DEVICE_DISCONNECTED_C;
        bool connected = false;
        if (i < deviceCount && sensors.getAddress(addr, i))
        {
            temp = sensors.getTempC(addr);
            connected = true;
        }
        int logicalId = getSensorLogicalId(i);
        sensorRows.push_back({i, logicalId, {0}, temp, connected});
        memcpy(sensorRows.back().addr, addr, sizeof(DeviceAddress));
    }
    // 연결된 센서만 논리 ID 기준 오름차순 정렬, 미연결 센서는 뒤로
    std::sort(sensorRows.begin(), sensorRows.end(), [](const SensorRowInfo &a, const SensorRowInfo &b)
              {
        if (a.connected != b.connected) return a.connected > b.connected;
        if (!a.connected && !b.connected) return a.idx < b.idx;
        return a.logicalId < b.logicalId; });

    // 정렬 결과를 전역 배열에 저장 (센서 선택 시 사용)
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        g_sortedSensorRows[i] = sensorRows[i];
    }

    // 1~8번 행을 모두 출력: 정렬된 센서, 그 뒤 미연결 센서
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        const auto &row = g_sortedSensorRows[i];
        if (row.connected)
        {
            if (row.logicalId < 1 || row.logicalId > SENSOR_MAX_COUNT)
            {
                idErrorFound = true;
                idErrorList += String(row.idx + 1) + "(0x";
                for (uint8_t j = 0; j < 8; j++)
                {
                    if (row.addr[j] < 16)
                        idErrorList += "0";
                    idErrorList += String(row.addr[j], HEX);
                }
                idErrorList += ") ";
            }
            printSensorRow(row.idx, i + 1, row.addr, row.temp);
        }
        else
        {
            DeviceAddress dummy = {0};
            printSensorRow(-1, i + 1, dummy, DEVICE_DISCONNECTED_C);
        }
    }
    Serial.println("=================================================================================================================");
    Serial.println("=================================================================================================================");
    if (idErrorFound)
    {
        Serial.print("[경고] 유효하지 않은 센서 ID(1~8 범위 밖) 감지: 센서 번호/주소: ");
        Serial.println(idErrorList);
        Serial.println("각 센서의 논리 ID(alarmHigh)는 반드시 1~8 범위여야 합니다. 메뉴에서 ID를 재설정하세요.");
    }
    Serial.println("센서 제어 메뉴 진입: 'menu' 또는 'm' 입력");
    Serial.println("(센서 ID/임계값/상태 관리 등은 메뉴에서 설정 가능)");
}

void printMenu()
{
    Serial.println();
    Serial.println("===== 센서 제어 메뉴 =====");
    Serial.println("1. 센서 ID 조정");
    Serial.println("2. 상/하한 온도 조정");
    Serial.println("3. 취소 / 상태창으로 돌아가기");
    Serial.print("메뉴 번호를 입력하세요: ");
}

// 센서 ID 조정 하위 메뉴
void printSensorIdMenu()
{
    Serial.println();
    Serial.println("--- 센서 ID 조정 메뉴 ---");
    Serial.println("1. 개별 센서 ID 변경");
    Serial.println("2. 복수의 센서 ID 변경");
    Serial.println("3. 주소순 자동 ID 할당");
    Serial.println("4. 전체 ID 초기화");
    Serial.println("5. 이전 메뉴 이동");
    Serial.println("6. 상태창으로 돌아가기");
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
                Serial.print("[DEBUG] appState: ");
                Serial.println((int)appState);
                Serial.print("[DEBUG] inputBuffer: ");
                Serial.println(inputBuffer);
                if (appState == AppState::Normal)
                {
                    if (inputBuffer == "menu" || inputBuffer == "m")
                    {
                        appState = AppState::Menu;
                        Serial.println("[DEBUG] appState -> Menu");
                        printMenu();
                    }
                }
                else if (appState == AppState::Menu)
                {
                    if (inputBuffer == "1")
                    {
                        appState = AppState::SensorIdMenu;
                        Serial.println("[DEBUG] appState -> SensorIdMenu");
                        printSensorIdMenu();
                    }
                    else if (inputBuffer == "2")
                    {
                        Serial.println("[상/하한 온도 조정 메뉴는 추후 구현]");
                        printMenu();
                    }
                    else if (inputBuffer == "3")
                    {
                        appState = AppState::Normal;
                        Serial.println("[DEBUG] appState -> Normal");
                        printSensorStatusTable();
                        lastPrint = millis();
                    }
                    else
                    {
                        Serial.println("지원하지 않는 메뉴입니다. 1~3 중 선택하세요.");
                        printMenu();
                    }
                }
                else if (appState == AppState::SensorIdMenu)
                {
                    if (inputBuffer == "1")
                    {
                        appState = AppState::SensorIdChange_SelectSensor;
                        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor");
                        Serial.println("[개별 센서 ID 변경] 센서 상태창:");
                        printSensorStatusTable();
                        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
                    }
                    else if (inputBuffer == "3")
                    {
                        // 주소순 자동 ID 할당
                        assignIDsByAddress();
                        Serial.println("[자동] 주소순 ID 할당 완료");
                        printSensorIdMenu();
                    }
                    else if (inputBuffer == "5")
                    {
                        appState = AppState::Menu;
                        Serial.println("[DEBUG] appState -> Menu");
                        printMenu();
                    }
                    else if (inputBuffer == "6")
                    {
                        appState = AppState::Normal;
                        Serial.println("[DEBUG] appState -> Normal");
                        printSensorStatusTable();
                        lastPrint = millis();
                    }
                    else
                    {
                        Serial.println("지원하지 않는 메뉴입니다. 1, 5: 이전, 6: 상태창으로 돌아가기");
                        printSensorIdMenu();
                    }
                }
                else if (appState == AppState::SensorIdChange_SelectSensor)
                {
                    if (inputBuffer == "c" || inputBuffer == "C")
                    {
                        appState = AppState::SensorIdMenu;
                        Serial.println("[DEBUG] appState -> SensorIdMenu");
                        printSensorIdMenu();
                    }
                    else
                    {
                        int sensorIdx = inputBuffer.toInt();
                        Serial.print("[DEBUG] sensorIdx: ");
                        Serial.println(sensorIdx);
                        if (sensorIdx >= 1 && sensorIdx <= SENSOR_MAX_COUNT && g_sortedSensorRows[sensorIdx - 1].connected)
                        {
                            selectedDisplayIdx = sensorIdx;                            // 사용자 선택 번호 저장
                            selectedSensorIdx = g_sortedSensorRows[sensorIdx - 1].idx; // 물리 인덱스 저장
                            Serial.print("[DEBUG] selectedDisplayIdx (번호): ");
                            Serial.println(selectedDisplayIdx);
                            Serial.print("[DEBUG] selectedSensorIdx (물리 인덱스): ");
                            Serial.println(selectedSensorIdx);
                            appState = AppState::SensorIdChange_ConfirmSensor;
                            Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor");
                            Serial.print("센서 ");
                            Serial.print(selectedDisplayIdx);
                            Serial.println("번을 변경할까요? (y/n, 취소:c)");
                        }
                        else
                        {
                            Serial.println("[오류] 연결된 센서만 선택할 수 있습니다. (1~8)");
                            Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
                        }
                    }
                }
                else if (appState == AppState::SensorIdChange_ConfirmSensor)
                {
                    if (inputBuffer == "y" || inputBuffer == "Y")
                    {
                        appState = AppState::SensorIdChange_InputId;
                        Serial.println("[DEBUG] appState -> SensorIdChange_InputId");
                        Serial.print("센서 ");
                        Serial.print(selectedDisplayIdx);
                        Serial.println("의 새로운 ID(1~8, 취소:c)를 입력하세요: ");
                    }
                    else if (inputBuffer == "n" || inputBuffer == "N")
                    {
                        appState = AppState::SensorIdChange_SelectSensor;
                        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor");
                        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
                    }
                    else if (inputBuffer == "c" || inputBuffer == "C")
                    {
                        appState = AppState::SensorIdMenu;
                        Serial.println("[DEBUG] appState -> SensorIdMenu");
                        printSensorIdMenu();
                    }
                    else
                    {
                        Serial.println("y(예), n(아니오), c(취소) 중 하나를 입력하세요.");
                        Serial.print("센서 ");
                        Serial.print(selectedDisplayIdx);
                        Serial.println("번을 변경할까요? (y/n, 취소:c)");
                    }
                }
                else if (appState == AppState::SensorIdChange_InputId)
                {
                    if (inputBuffer == "c" || inputBuffer == "C")
                    {
                        appState = AppState::SensorIdMenu;
                        Serial.println("[DEBUG] appState -> SensorIdMenu (취소)");
                        printSensorIdMenu();
                    }
                    else
                    {
                        int newId = inputBuffer.toInt();
                        Serial.print("[DEBUG] 입력된 newId: ");
                        Serial.println(newId);
                        if (newId >= 1 && newId <= SENSOR_MAX_COUNT)
                        {
                            // 중복 체크
                            if (isIdDuplicated(newId, selectedSensorIdx))
                            {
                                Serial.println("[오류] 이미 사용 중인 ID입니다. 다른 값을 입력하세요 (취소:c)");
                            }
                            else
                            {
                                setSensorLogicalId(selectedSensorIdx, newId);
                                Serial.print("센서 ");
                                Serial.print(selectedDisplayIdx);
                                Serial.print("의 ID를 ");
                                Serial.print(newId);
                                Serial.println("(으)로 변경 완료 (센서 EEPROM 저장)");
                                delay(30); // ensure EEPROM write is complete
                                printSensorStatusTable();
                                appState = AppState::SensorIdMenu;
                                Serial.println("[DEBUG] appState -> SensorIdMenu (변경 후)");
                                printSensorIdMenu();
                            }
                        }
                        else
                        {
                            Serial.println("[오류] ID는 1~8 사이의 숫자여야 합니다. 다시 입력하세요 (취소:c)");
                        }
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
