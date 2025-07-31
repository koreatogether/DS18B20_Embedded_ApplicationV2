#ifndef I_HAL_H
#define I_HAL_H

#include <cstdint>

/**
 * @class IHal
 * @brief Hardware Abstraction Layer (HAL) 인터페이스.
 * @details Arduino 프레임워크와 같이 하드웨어에 직접 의존하는 함수들을 추상화합니다.
 *          이를 통해 다른 클래스들이 하드웨어에 직접 의존하지 않고,
 *          이 인터페이스에만 의존하도록 하여 테스트 용이성을 높입니다.
 */
class IHal
{
public:
    virtual ~IHal() = default;

    /**
     * @brief 시스템 시작 후 경과된 시간을 밀리초 단위로 반환합니다. (millis()의 추상화)
     * @return unsigned long 경과 시간 (ms)
     */
    virtual unsigned long millis() const = 0;

    /**
     * @brief 현재 가용 메모리(free memory)의 바이트 수를 반환합니다.
     * @return int 가용 메모리 크기
     */
    virtual int getFreeMemoryBytes() const = 0;

    /**
     * @brief 메모리 구조 정보를 담은 문자열을 반환합니다.
     * @return std::string 메모리 구조 정보
     */
    virtual std::string getMemoryStructureInfo() const = 0;

    /**
     * @brief 주어진 문자열을 출력합니다. (Serial.println()의 추상화)
     * @param message 출력할 메시지
     */
    virtual void print(const std::string &message) = 0;
};

#endif // I_HAL_H
