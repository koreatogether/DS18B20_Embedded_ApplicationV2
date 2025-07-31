#ifndef MOCK_MEMORY_MONITOR_SERVICE_H
#define MOCK_MEMORY_MONITOR_SERVICE_H

#include "../../src/application/IMemoryAnalyzer.h"
#include <string>
#include <sstream>
#include <vector>

/**
 * @class MockMemoryMonitorService
 * @brief IMemoryAnalyzer 인터페이스의 완전한 Header-Only Mock 구현체
 * @details 실제 MemoryMonitorService의 모든 비즈니스 로직을 포함하되,
 *          하드웨어 의존성은 Mock으로 대체한 완전 독립적인 테스트용 구현
 */
class MockMemoryMonitorService : public IMemoryAnalyzer
{
private:
    // Mock 하드웨어 상태
    unsigned long _currentMillis;
    int _freeMemoryBytes;
    std::string _memoryStructureInfo;
    std::vector<std::string> _printedMessages;

    // 서비스 상태 (실제 구현과 동일)
    unsigned long _lastCheckTime;
    const unsigned long _interval;
    bool _isMonitoringEnabled;

public:
    MockMemoryMonitorService(unsigned long interval = 1000)
        : _currentMillis(0),
          _freeMemoryBytes(1024),
          _memoryStructureInfo("Heap End: 0x200, Stack Pointer: 0x300"),
          _lastCheckTime(0),
          _interval(interval),
          _isMonitoringEnabled(true) {}

    // Test Control Methods - 테스트에서 Mock 상태를 제어하기 위함
    void setCurrentMillis(unsigned long millis) { _currentMillis = millis; }
    void setFreeMemoryBytes(int bytes) { _freeMemoryBytes = bytes; }
    void setMemoryStructureInfo(const std::string &info) { _memoryStructureInfo = info; }
    const std::vector<std::string> &getPrintedMessages() const { return _printedMessages; }
    void clearPrintedMessages() { _printedMessages.clear(); }

    // IMemoryAnalyzer 인터페이스 구현 (실제 비즈니스 로직과 동일)
    void periodicCheck() override
    {
        if (_isMonitoringEnabled && (_currentMillis - _lastCheckTime >= _interval))
        {
            _lastCheckTime = _currentMillis;

            // 주기적 메모리 모니터링 로그 생성 (향상된 버전)
            std::string freeMemLog = getFreeMemory();
            std::string structLog = getStructureAnalysis();

            // CSV 형식으로 타임스탬프와 함께 로그 출력
            std::string periodicLog = formatAsCsv("PERIODIC_CHECK", _currentMillis) +
                                      "," + freeMemLog + "," + structLog;
            _printedMessages.push_back(periodicLog);
        }
    }

    std::string getRuntimeAnalysis() override
    {
        // 런타임 메모리 분석: 동적 메모리 할당/해제 테스트 시뮬레이션
        int initialFree = _freeMemoryBytes;

        // 스트레스 테스트 시뮬레이션: 메모리 사용량 변화
        int stressFree = _freeMemoryBytes - 400; // 100개 int * 4바이트 시뮬레이션
        int finalFree = _freeMemoryBytes;        // 메모리 해제 후 원상복구

        // 결과를 CSV 형식으로 반환
        std::ostringstream result;
        result << formatAsCsv("RUNTIME_INITIAL", initialFree) << ","
               << formatAsCsv("RUNTIME_STRESS", stressFree) << ","
               << formatAsCsv("RUNTIME_FINAL", finalFree) << ","
               << formatAsCsv("RUNTIME_DIFF", finalFree - initialFree);

        return result.str();
    }

    std::string getStructureAnalysis() override
    {
        return formatAsMarkdown("MEMORY_STRUCTURE", _memoryStructureInfo);
    }

    std::string getFreeMemory() override
    {
        return formatAsCsv("FREE_MEMORY", _freeMemoryBytes);
    }

    std::string toggleMonitoring() override
    {
        _isMonitoringEnabled = !_isMonitoringEnabled;
        std::string status = _isMonitoringEnabled ? "ENABLED" : "DISABLED";
        return formatAsMarkdown("MONITORING_STATUS", status);
    }

    std::string toggleTracking() override
    {
        // Mock implementation for memory tracking toggle
        static bool trackingEnabled = false;
        trackingEnabled = !trackingEnabled;
        std::string status = trackingEnabled ? "ENABLED" : "DISABLED";
        return formatAsMarkdown("TRACKING_STATUS", status);
    }

    std::string getMemoryReport() override
    {
        // Mock implementation for memory report
        std::ostringstream oss;
        oss << "# Mock Memory Tracking Report\n\n";
        oss << "- **Total Snapshots**: 5\n";
        oss << "- **Min Free Memory**: " << (_freeMemoryBytes - 200) << " bytes\n";
        oss << "- **Max Free Memory**: " << (_freeMemoryBytes + 200) << " bytes\n";
        oss << "- **Avg Free Memory**: " << _freeMemoryBytes << " bytes\n";
        oss << "- **Memory Trend**: Stable\n";
        oss << "- **Duration**: 5000 ms\n";
        return oss.str();
    }

private:
    // 실제 구현과 동일한 헬퍼 메서드들 (향상된 로그 형식)
    std::string formatAsCsv(const std::string &type, int value)
    {
        std::ostringstream oss;
        // CSV 헤더: timestamp,type,value
        oss << _currentMillis << "," << type << "," << value;
        return oss.str();
    }

    std::string formatAsMarkdown(const std::string &type, const std::string &value)
    {
        std::ostringstream oss;
        // Markdown 테이블 형식: | 시간 | 타입 | 값 |
        oss << "| " << _currentMillis << " | " << type << " | " << value << " |";
        return oss.str();
    }
};

#endif // MOCK_MEMORY_MONITOR_SERVICE_H
