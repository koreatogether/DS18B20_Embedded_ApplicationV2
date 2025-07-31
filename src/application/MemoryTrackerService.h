#ifndef MEMORY_TRACKER_SERVICE_H
#define MEMORY_TRACKER_SERVICE_H

#include "IMemoryTracker.h"
#include "../hal/IHal.h"
#include <memory>
#include <vector>
#include <deque>

/**
 * @class MemoryTrackerService
 * @brief IMemoryTracker 인터페이스의 구현체
 * @details 메모리 사용량 변화 이력을 저장하고 분석하는 서비스
 */
class MemoryTrackerService : public IMemoryTracker
{
public:
    /**
     * @brief 생성자
     * @param hal 하드웨어 추상화 계층
     * @param maxHistorySize 최대 히스토리 크기 (기본: 100)
     */
    MemoryTrackerService(std::shared_ptr<IHal> hal, int maxHistorySize = 100);
    virtual ~MemoryTrackerService() = default;

    // IMemoryTracker 인터페이스 구현
    void recordSnapshot(int freeBytes, int usedBytes, const std::string &eventType, const std::string &description = "") override;
    void recordCurrentMemory(const std::string &eventType, const std::string &description = "") override;
    std::vector<MemorySnapshot> getSnapshots() const override;
    MemoryReport generateReport() const override;
    std::string exportToCsv() const override;
    std::string exportReportToMarkdown() const override;
    void clearHistory() override;
    void setMaxHistorySize(int maxSize) override;

    /**
     * @brief 저장된 스냅샷 수 조회
     * @return 스냅샷 수
     */
    int getSnapshotCount() const;

private:
    std::shared_ptr<IHal> _hal;
    std::deque<MemorySnapshot> _snapshots; // 원형 버퍼 역할
    int _maxHistorySize;

    /**
     * @brief 메모리 트렌드 계산 (-1: 감소, 0: 안정, 1: 증가)
     * @return 트렌드 값
     */
    int calculateTrend() const;

    /**
     * @brief 히스토리 크기 제한 적용
     */
    void enforceHistoryLimit();
};

#endif // MEMORY_TRACKER_SERVICE_H
