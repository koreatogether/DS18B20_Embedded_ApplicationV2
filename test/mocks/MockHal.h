#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include "../../src/hal/IHal.h"
#include <string>
#include <vector>

/**
 * @class MockHal
 * @brief IHal 인터페이스의 Mock 구현체 (테스트용)
 * @details 실제 하드웨어 호출 대신 테스트 가능한 값들을 반환하며,
 *          호출된 메서드들을 기록하여 테스트에서 검증할 수 있게 합니다.
 */
class MockHal : public IHal
{
private:
    unsigned long _currentMillis;
    int _freeMemoryBytes;
    std::string _memoryStructureInfo;
    std::vector<std::string> _printedMessages;

public:
    MockHal()
        : _currentMillis(0),
          _freeMemoryBytes(1024),
          _memoryStructureInfo("Heap End: 0x200, Stack Pointer: 0x300") {}

    // Test Setters - 테스트에서 Mock 동작을 설정하기 위함
    void setCurrentMillis(unsigned long millis) { _currentMillis = millis; }
    void setFreeMemoryBytes(int bytes) { _freeMemoryBytes = bytes; }
    void setMemoryStructureInfo(const std::string &info) { _memoryStructureInfo = info; }

    // Test Getters - 테스트에서 호출 결과를 검증하기 위함
    const std::vector<std::string> &getPrintedMessages() const { return _printedMessages; }
    void clearPrintedMessages() { _printedMessages.clear(); }

    // IHal 인터페이스 구현
    unsigned long millis() const override
    {
        return _currentMillis;
    }

    int getFreeMemoryBytes() const override
    {
        return _freeMemoryBytes;
    }

    std::string getMemoryStructureInfo() const override
    {
        return _memoryStructureInfo;
    }

    void print(const std::string &message) override
    {
        _printedMessages.push_back(message);
    }
};

#endif // MOCK_HAL_H
