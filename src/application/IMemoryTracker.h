#ifndef IMEMORY_TRACKER_H
#define IMEMORY_TRACKER_H

#include <string>
#include <vector>

/**
 * @struct MemorySnapshot
 * @brief 특정 시점의 메모리 상태를 담는 구조체
 */
struct MemorySnapshot
{
    unsigned long timestamp; // 측정 시간 (millis)
    int freeBytes;           // 자유 메모리 바이트
    int usedBytes;           // 사용된 메모리 바이트 (계산값)
    std::string eventType;   // 이벤트 타입 ("INIT", "PERIODIC", "STRESS", "MANUAL")
    std::string description; // 추가 설명

    MemorySnapshot(unsigned long ts, int free, int used, const std::string &type, const std::string &desc)
        : timestamp(ts), freeBytes(free), usedBytes(used), eventType(type), description(desc) {}
};

/**
 * @struct MemoryReport
 * @brief 메모리 변화 분석 리포트 구조체
 */
struct MemoryReport
{
    int totalSnapshots;     // 총 스냅샷 수
    int minFreeBytes;       // 최소 자유 메모리
    int maxFreeBytes;       // 최대 자유 메모리
    int avgFreeBytes;       // 평균 자유 메모리
    int memoryTrend;        // 메모리 트렌드 (-1: 감소, 0: 안정, 1: 증가)
    unsigned long duration; // 추적 기간 (ms)
    std::string summary;    // 요약 텍스트
};

/**
 * @interface IMemoryTracker
 * @brief 메모리 사용량 변화 추적 및 리포트 생성 인터페이스
 * @details Application Layer에서 메모리 변화 이력을 관리하고 분석하는 책임을 담당
 */
class IMemoryTracker
{
public:
    virtual ~IMemoryTracker() = default;

    /**
     * @brief 현재 메모리 상태를 스냅샷으로 저장
     * @param freeBytes 자유 메모리 바이트
     * @param usedBytes 사용된 메모리 바이트
     * @param eventType 이벤트 타입
     * @param description 추가 설명
     */
    virtual void recordSnapshot(int freeBytes, int usedBytes, const std::string &eventType, const std::string &description = "") = 0;

    /**
     * @brief 현재 메모리 상태를 자동으로 측정하여 스냅샷으로 저장
     * @param eventType 이벤트 타입
     * @param description 추가 설명
     */
    virtual void recordCurrentMemory(const std::string &eventType, const std::string &description = "") = 0;

    /**
     * @brief 저장된 모든 스냅샷 조회
     * @return 메모리 스냅샷 벡터
     */
    virtual std::vector<MemorySnapshot> getSnapshots() const = 0;

    /**
     * @brief 메모리 변화 분석 리포트 생성
     * @return MemoryReport 구조체
     */
    virtual MemoryReport generateReport() const = 0;

    /**
     * @brief 스냅샷 데이터를 CSV 형식으로 출력
     * @return CSV 형식 문자열
     */
    virtual std::string exportToCsv() const = 0;

    /**
     * @brief 리포트를 Markdown 형식으로 출력
     * @return Markdown 형식 문자열
     */
    virtual std::string exportReportToMarkdown() const = 0;

    /**
     * @brief 저장된 스냅샷 데이터 초기화
     */
    virtual void clearHistory() = 0;

    /**
     * @brief 최대 저장 가능한 스냅샷 수 설정
     * @param maxSize 최대 크기 (0 = 무제한)
     */
    virtual void setMaxHistorySize(int maxSize) = 0;
};

#endif // IMEMORY_TRACKER_H
