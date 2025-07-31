#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "ITemperatureSensor.h"

// DS18B20 센서 에러 코드 정의
enum class SensorErrorCode
{
    SUCCESS = 0,
    SENSOR_NOT_FOUND = -1,
    INDEX_OUT_OF_RANGE = -2,
    TEMPERATURE_READ_ERROR = -3,
    SENSOR_DISCONNECTED = -4,
    INVALID_CRC = -5,
    POWER_PARASITE_ERROR = -6
};

// DS18B20 센서 관리 도메인 매니저
// 여러 센서의 주소, ID, 온도값을 관리하며, 센서 자동 탐지 및 예외 처리 기능 제공
class TemperatureSensorManager
{
public:
    TemperatureSensorManager();
    ~TemperatureSensorManager();

    void discoverSensors(); // 센서 자동 탐지
    size_t getSensorCount() const;
    bool getSensorAddress(size_t index, uint8_t *address) const;
    float readTemperature(size_t index);
    bool setSensorId(size_t index, const std::string &id);
    std::string getSensorId(size_t index) const;
    bool isIdDuplicated(const std::string &id) const;

    // 에러 처리 및 예외 상황 관리 메서드
    bool isTemperatureValid(float temperature) const;
    SensorErrorCode getLastErrorCode() const;
    std::string getErrorMessage(SensorErrorCode errorCode) const;
    bool isSensorConnected(size_t index) const;
    void clearLastError();

    // [TEST ONLY] 테스트를 위한 센서 주소/ID 강제 주입 메서드
    void _test_setSensorAddresses(const std::vector<std::vector<uint8_t>> &addrs)
    {
        sensorAddresses_ = addrs;
        sensorIds_.resize(addrs.size());
    }
    void _test_setSensorIds(const std::vector<std::string> &ids) { sensorIds_ = ids; }
    void _test_setLastError(SensorErrorCode errorCode) { lastErrorCode_ = errorCode; }

private:
    std::vector<std::vector<uint8_t>> sensorAddresses_; // 8바이트 주소
    std::vector<std::string> sensorIds_;
    ITemperatureSensor *sensorDriver_; // 의존성 주입
    SensorErrorCode lastErrorCode_;    // 마지막 에러 코드

    // 내부 에러 처리 메서드
    void setLastError(SensorErrorCode errorCode);
    bool isValidIndex(size_t index) const;
};
