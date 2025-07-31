#pragma once

// 최대 센서 개수 정의
#define MAX_MOCK_SENSORS 10
#define MAX_ID_LENGTH 20

// DS18B20 센서 에러 코드 정의 (목업용)
enum class MockSensorErrorCode
{
    SUCCESS = 0,
    SENSOR_NOT_FOUND = -1,
    INDEX_OUT_OF_RANGE = -2,
    TEMPERATURE_READ_ERROR = -3,
    SENSOR_DISCONNECTED = -4,
    INVALID_CRC = -5,
    POWER_PARASITE_ERROR = -6
};

// TemperatureSensorManager의 목업 구현 (Header-Only)
// STL 없이 고정 배열만 사용하는 순수 목업
class MockTemperatureSensorManager
{
public:
    inline MockTemperatureSensorManager();
    ~MockTemperatureSensorManager() = default;

    inline void discoverSensors(); // 센서 자동 탐지 (목업)
    inline int getSensorCount() const;
    inline bool getSensorAddress(int index, unsigned char *address) const;
    inline float readTemperature(int index);
    inline bool setSensorId(int index, const char *id);
    inline const char *getSensorId(int index) const;
    inline bool isIdDuplicated(const char *id) const;

    // 에러 처리 및 예외 상황 관리 메서드
    inline bool isTemperatureValid(float temperature) const;
    inline MockSensorErrorCode getLastErrorCode() const;
    inline const char *getErrorMessage(MockSensorErrorCode errorCode) const;
    inline bool isSensorConnected(int index) const;
    inline void clearLastError();

    // 목업 테스트를 위한 설정 메서드들
    inline void setMockSensorCount(int count);
    inline void setMockTemperature(int index, float temperature);
    inline void setMockSensorConnected(int index, bool connected);
    inline void setMockErrorCode(MockSensorErrorCode errorCode);

private:
    int sensorCount_;
    unsigned char sensorAddresses_[MAX_MOCK_SENSORS][8]; // 8바이트 주소
    char sensorIds_[MAX_MOCK_SENSORS][MAX_ID_LENGTH + 1];
    bool sensorIdSet_[MAX_MOCK_SENSORS];
    float temperatures_[MAX_MOCK_SENSORS];
    bool sensorConnected_[MAX_MOCK_SENSORS];
    MockSensorErrorCode lastErrorCode_;

    // 내부 메서드
    inline void setLastError(MockSensorErrorCode errorCode);
    inline bool isValidIndex(int index) const;
    inline int strlen_mock(const char *str) const;
    inline void strcpy_mock(char *dest, const char *src);
    inline bool strcmp_mock(const char *str1, const char *str2) const;
};

// --- 구현부 ---

inline MockTemperatureSensorManager::MockTemperatureSensorManager()
    : sensorCount_(0), lastErrorCode_(MockSensorErrorCode::SUCCESS)
{
    // 배열 초기화
    for (int i = 0; i < MAX_MOCK_SENSORS; i++)
    {
        // 센서 주소 초기화
        for (int j = 0; j < 8; j++)
        {
            sensorAddresses_[i][j] = 0x00;
        }

        // ID 초기화
        sensorIds_[i][0] = '\0';
        sensorIdSet_[i] = false;

        // 온도 및 연결 상태 초기화
        temperatures_[i] = 0.0f;
        sensorConnected_[i] = true;
    }
}

inline void MockTemperatureSensorManager::discoverSensors()
{
    // 목업에서는 미리 설정된 센서 개수를 그대로 사용
    clearLastError();
}

inline int MockTemperatureSensorManager::getSensorCount() const
{
    return sensorCount_;
}

inline bool MockTemperatureSensorManager::getSensorAddress(int index, unsigned char *address) const
{
    if (!isValidIndex(index) || address == nullptr)
    {
        return false;
    }

    for (int i = 0; i < 8; i++)
    {
        address[i] = sensorAddresses_[index][i];
    }
    return true;
}

inline float MockTemperatureSensorManager::readTemperature(int index)
{
    if (!isValidIndex(index))
    {
        setLastError(MockSensorErrorCode::INDEX_OUT_OF_RANGE);
        return -127.0f;
    }

    if (!sensorConnected_[index])
    {
        setLastError(MockSensorErrorCode::SENSOR_DISCONNECTED);
        return -127.0f;
    }

    clearLastError();
    return temperatures_[index];
}

inline bool MockTemperatureSensorManager::setSensorId(int index, const char *id)
{
    if (!isValidIndex(index) || id == nullptr)
    {
        return false;
    }

    if (strlen_mock(id) > MAX_ID_LENGTH)
    {
        return false;
    }

    // ID 중복 체크
    if (isIdDuplicated(id))
    {
        return false;
    }

    strcpy_mock(sensorIds_[index], id);
    sensorIdSet_[index] = true;
    return true;
}

inline const char *MockTemperatureSensorManager::getSensorId(int index) const
{
    if (!isValidIndex(index) || !sensorIdSet_[index])
    {
        return nullptr;
    }
    return sensorIds_[index];
}

inline bool MockTemperatureSensorManager::isIdDuplicated(const char *id) const
{
    if (id == nullptr)
    {
        return false;
    }

    for (int i = 0; i < sensorCount_; i++)
    {
        if (sensorIdSet_[i] && strcmp_mock(sensorIds_[i], id))
        {
            return true;
        }
    }
    return false;
}

inline bool MockTemperatureSensorManager::isTemperatureValid(float temperature) const
{
    return temperature > -55.0f && temperature < 125.0f;
}

inline MockSensorErrorCode MockTemperatureSensorManager::getLastErrorCode() const
{
    return lastErrorCode_;
}

inline const char *MockTemperatureSensorManager::getErrorMessage(MockSensorErrorCode errorCode) const
{
    switch (errorCode)
    {
    case MockSensorErrorCode::SUCCESS:
        return "Success";
    case MockSensorErrorCode::SENSOR_NOT_FOUND:
        return "Sensor not found";
    case MockSensorErrorCode::INDEX_OUT_OF_RANGE:
        return "Index out of range";
    case MockSensorErrorCode::TEMPERATURE_READ_ERROR:
        return "Temperature read error";
    case MockSensorErrorCode::SENSOR_DISCONNECTED:
        return "Sensor disconnected";
    case MockSensorErrorCode::INVALID_CRC:
        return "Invalid CRC";
    case MockSensorErrorCode::POWER_PARASITE_ERROR:
        return "Power/Parasite error";
    default:
        return "Unknown error";
    }
}

inline bool MockTemperatureSensorManager::isSensorConnected(int index) const
{
    if (!isValidIndex(index))
    {
        return false;
    }
    return sensorConnected_[index];
}

inline void MockTemperatureSensorManager::clearLastError()
{
    lastErrorCode_ = MockSensorErrorCode::SUCCESS;
}

// 목업 테스트를 위한 설정 메서드들
inline void MockTemperatureSensorManager::setMockSensorCount(int count)
{
    if (count >= 0 && count <= MAX_MOCK_SENSORS)
    {
        sensorCount_ = count;

        // 새로 추가된 센서들의 기본 주소 설정
        for (int i = 0; i < count; i++)
        {
            sensorAddresses_[i][0] = 0x28; // DS18B20 family code
            for (int j = 1; j < 7; j++)
            {
                sensorAddresses_[i][j] = static_cast<unsigned char>(i + j); // 임의의 주소
            }
            sensorAddresses_[i][7] = 0xFF; // CRC (임의)
        }
    }
}

inline void MockTemperatureSensorManager::setMockTemperature(int index, float temperature)
{
    if (isValidIndex(index))
    {
        temperatures_[index] = temperature;
    }
}

inline void MockTemperatureSensorManager::setMockSensorConnected(int index, bool connected)
{
    if (isValidIndex(index))
    {
        sensorConnected_[index] = connected;
    }
}

inline void MockTemperatureSensorManager::setMockErrorCode(MockSensorErrorCode errorCode)
{
    lastErrorCode_ = errorCode;
}

// 내부 메서드들
inline void MockTemperatureSensorManager::setLastError(MockSensorErrorCode errorCode)
{
    lastErrorCode_ = errorCode;
}

inline bool MockTemperatureSensorManager::isValidIndex(int index) const
{
    return index >= 0 && index < sensorCount_;
}

inline int MockTemperatureSensorManager::strlen_mock(const char *str) const
{
    if (str == nullptr)
    {
        return 0;
    }

    int len = 0;
    while (str[len] != '\0' && len < MAX_ID_LENGTH)
    {
        len++;
    }
    return len;
}

inline void MockTemperatureSensorManager::strcpy_mock(char *dest, const char *src)
{
    if (dest == nullptr || src == nullptr)
    {
        return;
    }

    int i = 0;
    while (src[i] != '\0' && i < MAX_ID_LENGTH)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

inline bool MockTemperatureSensorManager::strcmp_mock(const char *str1, const char *str2) const
{
    if (str1 == nullptr || str2 == nullptr)
    {
        return false;
    }

    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0' && i < MAX_ID_LENGTH)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
        i++;
    }
    return str1[i] == str2[i];
}
