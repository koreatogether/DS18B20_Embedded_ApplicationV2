#pragma once

// ITemperatureSensor: DS18B20 등 온도 센서 추상 인터페이스
class ITemperatureSensor
{
public:
    virtual ~ITemperatureSensor() = default;

    // 센서 개수 반환
    virtual int getSensorCount() const = 0;

    // 인덱스별 온도값 반환 (섭씨)
    virtual float getTemperatureC(int index) const = 0;

    // 센서별 사용자 데이터(ID) get/set
    virtual int getUserData(int index) const = 0;
    virtual void setUserData(int index, int userData) = 0;

    // ID 중복 체크
    virtual bool isIdUsed(int id) const = 0;
};
