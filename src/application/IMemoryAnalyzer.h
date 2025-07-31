#ifndef I_MEMORY_ANALYZER_H
#define I_MEMORY_ANALYZER_H

#include <string>

/**
 * @class IMemoryAnalyzer
 * @brief 메모리 분석 시스템의 동작을 정의하는 인터페이스 (Application Layer)
 * @details 이 인터페이스는 메모리 상태 측정, 테스트, 로깅 등
 *          메모리 관련 유스케이스를 정의합니다.
 */
class IMemoryAnalyzer
{
public:
    virtual ~IMemoryAnalyzer() = default;

    /**
     * @brief 주기적으로 호출되어 메모리 상태를 모니터링합니다.
     * @details 내부적으로 시간(예: 10초)을 체크하여 로깅 등의 동작을 수행합니다.
     */
    virtual void periodicCheck() = 0;

    /**
     * @brief 런타임 메모리 테스트를 수행하고 결과를 문자열로 반환합니다.
     * @return std::string 테스트 결과 로그
     */
    virtual std::string getRuntimeAnalysis() = 0;

    /**
     * @brief 메모리 구조 분석을 수행하고 결과를 문자열로 반환합니다.
     * @return std::string 분석 결과 로그
     */
    virtual std::string getStructureAnalysis() = 0;

    /**
     * @brief 현재 가용 메모리(free memory) 정보를 반환합니다.
     * @return std::string 가용 메모리 정보 로그
     */
    virtual std::string getFreeMemory() = 0;

    /**
     * @brief 주기적인 메모리 모니터링 기능의 활성화/비활성화 상태를 토글합니다.
     * @return std::string 변경된 상태 정보
     */
    virtual std::string toggleMonitoring() = 0;

    /**
     * @brief 메모리 변화 추적 기능의 활성화/비활성화 상태를 토글합니다.
     * @return std::string 변경된 상태 정보
     */
    virtual std::string toggleTracking() = 0;

    /**
     * @brief 메모리 추적 리포트를 생성하여 반환합니다.
     * @return std::string 메모리 추적 리포트
     */
    virtual std::string getMemoryReport() = 0;
};

#endif // I_MEMORY_ANALYZER_H
