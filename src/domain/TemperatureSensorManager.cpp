#include "TemperatureSensorManager.h"

TemperatureSensorManager::TemperatureSensorManager()
    : sensorDriver_(nullptr), lastErrorCode_(SensorErrorCode::SUCCESS)
{
    // TODO: 의존성 주입 방식으로 sensorDriver_ 할당 예정
}

TemperatureSensorManager::~TemperatureSensorManager() {}

void TemperatureSensorManager::discoverSensors()
{
    // TODO: 센서 자동 탐지 로직 구현 예정
    clearLastError();
}

size_t TemperatureSensorManager::getSensorCount() const
{
    return sensorAddresses_.size();
}

bool TemperatureSensorManager::getSensorAddress(size_t index, uint8_t *address) const
{
    if (!isValidIndex(index))
    {
        const_cast<TemperatureSensorManager *>(this)->setLastError(SensorErrorCode::INDEX_OUT_OF_RANGE);
        return false;
    }

    for (size_t i = 0; i < 8; ++i)
        address[i] = sensorAddresses_[index][i];
    return true;
}

float TemperatureSensorManager::readTemperature(size_t index)
{
    if (!isValidIndex(index))
    {
        setLastError(SensorErrorCode::INDEX_OUT_OF_RANGE);
        return -127.0f;
    }

    if (!isSensorConnected(index))
    {
        setLastError(SensorErrorCode::SENSOR_DISCONNECTED);
        return -127.0f;
    }

    // TODO: 센서 드라이버를 통해 온도값 읽기 구현 예정
    // 현재는 에러값 반환 시뮬레이션
    setLastError(SensorErrorCode::TEMPERATURE_READ_ERROR);
    return -127.0f;
}

bool TemperatureSensorManager::setSensorId(size_t index, const std::string &id)
{
    if (!isValidIndex(index))
    {
        setLastError(SensorErrorCode::INDEX_OUT_OF_RANGE);
        return false;
    }
    sensorIds_[index] = id;
    clearLastError();
    return true;
}

std::string TemperatureSensorManager::getSensorId(size_t index) const
{
    if (!isValidIndex(index))
    {
        const_cast<TemperatureSensorManager *>(this)->setLastError(SensorErrorCode::INDEX_OUT_OF_RANGE);
        return "";
    }
    return sensorIds_[index];
}

bool TemperatureSensorManager::isIdDuplicated(const std::string &id) const
{
    for (const auto &existingId : sensorIds_)
    {
        if (existingId == id)
            return true;
    }
    return false;
}

// 에러 처리 및 예외 상황 관리 메서드 구현
bool TemperatureSensorManager::isTemperatureValid(float temperature) const
{
    // DS18B20 센서의 일반적인 에러값들 체크
    if (temperature <= -127.0f || temperature >= 125.0f)
        return false;

    // NaN 체크
    if (temperature != temperature)
        return false;

    return true;
}

SensorErrorCode TemperatureSensorManager::getLastErrorCode() const
{
    return lastErrorCode_;
}

std::string TemperatureSensorManager::getErrorMessage(SensorErrorCode errorCode) const
{
    switch (errorCode)
    {
    case SensorErrorCode::SUCCESS:
        return "Success";
    case SensorErrorCode::SENSOR_NOT_FOUND:
        return "Sensor not found";
    case SensorErrorCode::INDEX_OUT_OF_RANGE:
        return "Index out of range";
    case SensorErrorCode::TEMPERATURE_READ_ERROR:
        return "Temperature read error";
    case SensorErrorCode::SENSOR_DISCONNECTED:
        return "Sensor disconnected";
    case SensorErrorCode::INVALID_CRC:
        return "Invalid CRC";
    case SensorErrorCode::POWER_PARASITE_ERROR:
        return "Power/Parasite error";
    default:
        return "Unknown error";
    }
}

bool TemperatureSensorManager::isSensorConnected(size_t index) const
{
    if (!isValidIndex(index))
        return false;

    // TODO: 실제 센서 연결 상태 확인 로직 구현 예정
    // 임시: 인덱스가 유효하면 연결된 것으로 간주 (테스트/정적분석 경고 방지)
    return true;
}

void TemperatureSensorManager::clearLastError()
{
    lastErrorCode_ = SensorErrorCode::SUCCESS;
}

// 내부 에러 처리 메서드
void TemperatureSensorManager::setLastError(SensorErrorCode errorCode)
{
    lastErrorCode_ = errorCode;
}

bool TemperatureSensorManager::isValidIndex(size_t index) const
{
    return index < sensorAddresses_.size();
}
