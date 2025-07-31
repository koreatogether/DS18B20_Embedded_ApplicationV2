#ifndef MEMORY_MONITOR_SERVICE_H
#define MEMORY_MONITOR_SERVICE_H

#include "IMemoryAnalyzer.h"
#include "IMemoryTracker.h"
#include "../hal/IHal.h"
#include <memory>

/**
 * @class MemoryMonitorService
 * @brief IMemoryAnalyzer 인터페이스의 구현체 (Application Layer)
 * @details 메모리 분석 및 모니터링 유스케이스를 실제로 수행합니다.
 */
class MemoryMonitorService : public IMemoryAnalyzer
{
public:
    /**
     * @brief 생성자. HAL과 모니터링 간격을 설정합니다.
     * @param hal 하드웨어 추상화 계층 인터페이스
     * @param tracker 메모리 변화 추적기 (선택사항)
     * @param interval 주기적 모니터링 간격 (밀리초 단위)
     */
    MemoryMonitorService(std::shared_ptr<IHal> hal, std::shared_ptr<IMemoryTracker> tracker = nullptr, unsigned long interval = 10000);
    virtual ~MemoryMonitorService() = default;

    // IMemoryAnalyzer 인터페이스 구현
    void periodicCheck() override;
    std::string getRuntimeAnalysis() override;
    std::string getStructureAnalysis() override;
    std::string getFreeMemory() override;
    std::string toggleMonitoring() override;

    /**
     * @brief 메모리 변화 추적 기능 활성화/비활성화
     * @return 상태 메시지
     */
    std::string toggleTracking();

    /**
     * @brief 메모리 변화 리포트 생성
     * @return Markdown 형식의 리포트
     */
    std::string getMemoryReport();

private:
    /**
     * @brief 로그를 CSV 형식으로 생성합니다.
     * @param type 로그 타입 (e.g., "RUNTIME", "FREE")
     * @param value 측정된 값
     * @return std::string CSV 형식의 로그 문자열
     */
    std::string formatAsCsv(const std::string &type, int value);

    /**
     * @brief 로그를 Markdown 형식으로 생성합니다.
     * @param type 로그 타입
     * @param value 측정된 값
     * @return std::string Markdown 형식의 로그 문자열
     */
    std::string formatAsMarkdown(const std::string &type, const std::string &value);

    std::shared_ptr<IHal> _hal;               // 하드웨어 추상화 계층
    std::shared_ptr<IMemoryTracker> _tracker; // 메모리 변화 추적기
    unsigned long _lastCheckTime;             // 마지막 모니터링 시간
    const unsigned long _interval;            // 모니터링 간격
    bool _isMonitoringEnabled;                // 모니터링 활성화 여부
    bool _isTrackingEnabled;                  // 추적 기능 활성화 여부
};

#endif // MEMORY_MONITOR_SERVICE_H
