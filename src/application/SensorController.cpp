#include "SensorController.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <algorithm>

extern OneWire oneWire;
extern DallasTemperature sensors;

constexpr float UPPER_THRESHOLD = 30.0;
constexpr float LOWER_THRESHOLD = 20.0;

SensorRowInfo SensorController::g_sortedSensorRows[SENSOR_MAX_COUNT];

SensorController::SensorController()
{
    // 생성자에서 필요한 초기화 수행
}

uint8_t SensorController::getSensorLogicalId(int idx)
{
    int id = sensors.getUserDataByIndex(idx);
    if (id < 1 || id > SENSOR_MAX_COUNT)
    {
        return idx + 1;
    }
    return id;
}

void SensorController::setSensorLogicalId(int idx, uint8_t newId)
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

bool SensorController::isIdDuplicated(int newId, int exceptIdx)
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

void SensorController::assignIDsByAddress()
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

const char *SensorController::getUpperState(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "-";
    return (temp > UPPER_THRESHOLD) ? "초과" : "정상";
}

const char *SensorController::getLowerState(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "-";
    return (temp < LOWER_THRESHOLD) ? "초과" : "정상";
}

const char *SensorController::getSensorStatus(float temp)
{
    if (temp == DEVICE_DISCONNECTED_C)
        return "오류";
    return "정상";
}

void SensorController::printSensorAddress(const DeviceAddress &addr)
{
    Serial.print("0x");
    for (uint8_t j = 0; j < 8; j++)
    {
        if (addr[j] < 16)
            Serial.print("0");
        Serial.print(addr[j], HEX);
    }
}

void SensorController::printSensorRow(int idx, int id, const DeviceAddress &addr, float temp)
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

void SensorController::updateSensorRows()
{
    sensors.requestTemperatures();
    std::vector<SensorRowInfo> sensorRows;
    
    collectSensorData(sensorRows);
    sortSensorRows(sensorRows);
    storeSortedResults(sensorRows);
}

void SensorController::printSensorStatusTable()
{
    Serial.println("| 번호 | ID  | 센서 주소           | 현재 온도 | 상한설정온도 | 상한초과상태 | 하한설정온도 | 하한초과상태 | 센서상태 |");
    Serial.println("| ---- | --- | ------------       | ---------  | ------------  | ------------ | ------------ | ------------ | -------- |");

    updateSensorRows();

    bool idErrorFound = false;
    String idErrorList = "";

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
void SensorController::collectSensorData(std::vector<SensorRowInfo>& sensorRows)
{
    int deviceCount = sensors.getDeviceCount();
    
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        SensorRowInfo rowInfo = createSensorRowInfo(i, deviceCount);
        sensorRows.push_back(rowInfo);
    }
}

SensorRowInfo SensorController::createSensorRowInfo(int idx, int deviceCount)
{
    DeviceAddress addr = {0};
    float temp = DEVICE_DISCONNECTED_C;
    bool connected = false;
    
    if (idx < deviceCount && sensors.getAddress(addr, idx))
    {
        temp = sensors.getTempC(addr);
        connected = true;
    }
    
    int logicalId = getSensorLogicalId(idx);
    SensorRowInfo rowInfo = {idx, logicalId, {0}, temp, connected};
    
    // Copy address
    for (size_t k = 0; k < sizeof(DeviceAddress); ++k)
    {
        rowInfo.addr[k] = addr[k];
    }
    
    return rowInfo;
}

void SensorController::sortSensorRows(std::vector<SensorRowInfo>& sensorRows)
{
    // 연결된 센서만 논리 ID 기준 오름차순 정렬, 미연결 센서는 뒤로
    std::sort(sensorRows.begin(), sensorRows.end(), [](const SensorRowInfo &a, const SensorRowInfo &b)
    {
        if (a.connected != b.connected) return a.connected > b.connected;
        if (!a.connected && !b.connected) return a.idx < b.idx;
        return a.logicalId < b.logicalId;
    });
}

void SensorController::storeSortedResults(const std::vector<SensorRowInfo>& sensorRows)
{
    // 정렬 결과를 전역 배열에 저장
    for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
    {
        g_sortedSensorRows[i] = sensorRows[i];
    }
}