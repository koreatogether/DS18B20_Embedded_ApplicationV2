#ifndef ARDUINO_HAL_H
#define ARDUINO_HAL_H

#include "IHal.h"
#include <Arduino.h>
#include <sstream>
#include <iomanip>

// Arduino 환경에서만 사용되는 전역 변수 및 함수 선언
#ifdef __cplusplus
extern "C"
{
#endif
    extern char *__brkval;
    extern char *sbrk(int incr);
#ifdef __cplusplus
}
#endif

/**
 * @class ArduinoHal
 * @brief Arduino 프레임워크를 위한 IHal 인터페이스의 실제 구현체.
 * @details Arduino의 millis(), Serial.println() 등의 함수를 직접 호출하여
 *          하드웨어 제어 기능을 수행합니다.
 */
class ArduinoHal : public IHal
{
public:
    unsigned long millis() const override
    {
        return ::millis();
    }

    int getFreeMemoryBytes() const override
    {
        char top;
        return &top - __brkval;
    }

    std::string getMemoryStructureInfo() const override
    {
        std::ostringstream oss;
        oss << "Heap End: 0x" << std::hex << reinterpret_cast<uintptr_t>(__brkval)
            << ", Stack Pointer: 0x" << std::hex << reinterpret_cast<uintptr_t>(sbrk(0));
        return oss.str();
    }

    void print(const std::string &message) override
    {
        Serial.println(message.c_str());
    }
};

#endif // ARDUINO_HAL_H
