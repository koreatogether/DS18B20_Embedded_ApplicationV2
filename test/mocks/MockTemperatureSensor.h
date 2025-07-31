#pragma once

#include "..\interfaces\ITemperatureSensor.h"

// 최대 센서 개수 정의 (STL 없이 고정 배열 사용)
#define MAX_MOCK_SENSORS 10

/**
 * @brief ITemperatureSensor 인터페이스를 구현한 Mock 객체 (Header-Only)
 *
 * STL 라이브러리를 사용하지 않는 순수 목업 구현
 * 단위 테스트에서 실제 DS18B20 센서 하드웨어 없이도
 * TemperatureSensorManager를 테스트할 수 있도록 합니다.
 */
class MockTemperatureSensor : public ITemperatureSensor
{
public:
    inline MockTemperatureSensor();
    virtual ~MockTemperatureSensor() = default;

    // 테스트에서 설정할 수 있는 메서드들
    inline void setMockSensorCount(int count);
    inline void setMockTemperature(int index, float temperature);
    inline void setMockUserData(int index, int userData);
    inline void clearMockData();

    // ITemperatureSensor 인터페이스 구현
    inline int getSensorCount() const override;
    inline float getTemperatureC(int index) const override;
    inline int getUserData(int index) const override;
    inline void setUserData(int index, int userData) override;
    inline bool isIdUsed(int id) const override;

private:
    int sensorCount_;
    float temperatures_[MAX_MOCK_SENSORS];
    int userDataArray_[MAX_MOCK_SENSORS];
    bool userDataSet_[MAX_MOCK_SENSORS]; // 사용자 데이터 설정 여부 추적
};

// --- 구현부 ---

inline MockTemperatureSensor::MockTemperatureSensor()
    : sensorCount_(0)
{
    // 배열 초기화
    for (int i = 0; i < MAX_MOCK_SENSORS; i++)
    {
        temperatures_[i] = 0.0f;
        userDataArray_[i] = 0;
        userDataSet_[i] = false;
    }
}

inline void MockTemperatureSensor::setMockSensorCount(int count)
{
    if (count >= 0 && count <= MAX_MOCK_SENSORS)
    {
        sensorCount_ = count;
        // 새로 설정된 센서들은 기본값으로 초기화
        for (int i = 0; i < count; i++)
        {
            temperatures_[i] = 0.0f;
            userDataArray_[i] = 0;
            userDataSet_[i] = false;
        }
    }
}

inline void MockTemperatureSensor::setMockTemperature(int index, float temperature)
{
    if (index >= 0 && index < sensorCount_)
    {
        temperatures_[index] = temperature;
    }
}

inline void MockTemperatureSensor::setMockUserData(int index, int userData)
{
    if (index >= 0 && index < sensorCount_)
    {
        userDataArray_[index] = userData;
        userDataSet_[index] = true;
    }
}

inline void MockTemperatureSensor::clearMockData()
{
    sensorCount_ = 0;
    for (int i = 0; i < MAX_MOCK_SENSORS; i++)
    {
        temperatures_[i] = 0.0f;
        userDataArray_[i] = 0;
        userDataSet_[i] = false;
    }
}

inline int MockTemperatureSensor::getSensorCount() const
{
    return sensorCount_;
}

inline float MockTemperatureSensor::getTemperatureC(int index) const
{
    if (index < 0 || index >= sensorCount_)
    {
        return -127.0f; // 에러값 반환 (DS18B20 에러값)
    }
    return temperatures_[index];
}

inline int MockTemperatureSensor::getUserData(int index) const
{
    if (index >= 0 && index < sensorCount_ && userDataSet_[index])
    {
        return userDataArray_[index];
    }
    return 0; // 기본값 반환
}

inline void MockTemperatureSensor::setUserData(int index, int userData)
{
    if (index >= 0 && index < sensorCount_)
    {
        userDataArray_[index] = userData;
        userDataSet_[index] = true;
    }
}

inline bool MockTemperatureSensor::isIdUsed(int id) const
{
    for (int i = 0; i < sensorCount_; i++)
    {
        if (userDataSet_[i] && userDataArray_[i] == id)
        {
            return true;
        }
    }
    return false;
}
