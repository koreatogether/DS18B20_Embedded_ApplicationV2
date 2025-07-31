#pragma once

#include <cstring> // strncpy, strstr 함수 사용을 위해 추가

// STL 대체를 위한 고정 크기 정의
#define MAX_MEMORY_SNAPSHOTS 50
#define MAX_STRING_LENGTH 64

// --- 안전한 문자열 복사 전역 유틸리티 ---
inline void safeCopyString(char *dest, const char *src, int maxLength)
{
    if (!dest || maxLength <= 0)
        return;
    if (!src)
        src = "";
    int i = 0;
    for (; i < maxLength - 1 && src[i] != '\0'; ++i)
        dest[i] = src[i];
    dest[i] = '\0';
}

/**
 * @struct MemorySnapshotFixed
 * @brief Header-Only 환경을 위한 고정 크기 메모리 스냅샷
 */
struct MemorySnapshotFixed
{
    unsigned long timestamp;             // 측정 시간 (millis)
    int freeBytes;                       // 자유 메모리 바이트
    int usedBytes;                       // 사용된 메모리 바이트
    char eventType[MAX_STRING_LENGTH];   // 이벤트 타입
    char description[MAX_STRING_LENGTH]; // 추가 설명

    inline MemorySnapshotFixed()
        : timestamp(0), freeBytes(0), usedBytes(0)
    {
        eventType[0] = '\0';
        description[0] = '\0';
    }

    inline MemorySnapshotFixed(unsigned long ts, int free, int used,
                               const char *type, const char *desc)
        : timestamp(ts), freeBytes(free), usedBytes(used)
    {
        // 안전한 문자열 복사
        safeCopyString(eventType, type ? type : "", MAX_STRING_LENGTH);
        safeCopyString(description, desc ? desc : "", MAX_STRING_LENGTH);
    }
};

/**
 * @struct MemoryLeakAnalysis
 * @brief 메모리 누수 분석 결과 구조체
 */
struct MemoryLeakAnalysis
{
    bool leakDetected;                   // 누수 탐지 여부
    int suspiciousDecreaseCount;         // 의심스러운 메모리 감소 횟수
    int maxMemoryDecrease;               // 최대 메모리 감소량
    float averageMemoryTrend;            // 평균 메모리 변화 추세
    int totalSnapshots;                  // 전체 스냅샷 수
    char summary[MAX_STRING_LENGTH * 2]; // 분석 요약

    inline MemoryLeakAnalysis()
        : leakDetected(false), suspiciousDecreaseCount(0),
          maxMemoryDecrease(0), averageMemoryTrend(0.0f), totalSnapshots(0)
    {
        summary[0] = '\0';
    }
};

/**
 * @class MockMemoryLeakDetector
 * @brief Header-Only 메모리 누수 탐지 Mock 클래스
 *
 * 실제 Arduino 환경에서의 메모리 누수를 탐지하기 위한 테스트용 Mock 객체
 * STL을 사용하지 않고 고정 크기 배열로 구현하여 완전한 Header-Only 구조
 */
class MockMemoryLeakDetector
{
public:
    inline MockMemoryLeakDetector();
    virtual ~MockMemoryLeakDetector() = default;
    // 안전한 문자열 복사 유틸리티 (static) - deprecated, 전역 함수 사용 권장
    // static void safeCopyString(char *dest, const char *src, int maxLength);

    // 기본 메모리 추적 기능
    inline void recordSnapshot(int freeBytes, int usedBytes,
                               const char *eventType, const char *description = "");
    inline void recordCurrentMemory(const char *eventType, const char *description = "");
    inline void clearHistory();
    inline void setMaxHistorySize(int maxSize);

    // 메모리 누수 탐지 특화 기능
    inline void startLeakDetection(const char *testName);
    inline void recordTestIteration(int iteration, int freeMemory);
    inline MemoryLeakAnalysis analyzeLeakPattern();
    inline bool detectMemoryLeak(int thresholdBytes = 10, int minDecreaseCount = 3);

    // 스트레스 테스트 지원
    inline void simulateMemoryStress(int iterations, int memoryDecreasePerIteration = 2);
    inline void simulateMemoryRecovery(int recoveryBytes);

    // 리포트 생성
    inline int getSnapshotCount() const;
    inline MemorySnapshotFixed getSnapshot(int index) const;
    inline void generateLeakReport(char *outputBuffer, int bufferSize) const;

    // 테스트 유틸리티
    inline void setMockCurrentTime(unsigned long mockTime);
    inline void setMockFreeMemory(int mockFreeMemory);

private:
    MemorySnapshotFixed snapshots_[MAX_MEMORY_SNAPSHOTS];
    int snapshotCount_;
    int maxHistorySize_;

    // 누수 탐지를 위한 추가 데이터
    char currentTestName_[MAX_STRING_LENGTH];
    int baselineMemory_;
    bool leakDetectionActive_;

    // Mock 데이터 (테스트용)
    unsigned long mockCurrentTime_;
    int mockFreeMemory_;

    // 내부 유틸리티 함수들
    inline void addSnapshot(const MemorySnapshotFixed &snapshot);
    inline float calculateMemoryTrend() const;
    inline int findMaxMemoryDecrease() const;
};

// --- 구현부 ---

inline MockMemoryLeakDetector::MockMemoryLeakDetector()
    : snapshotCount_(0), maxHistorySize_(MAX_MEMORY_SNAPSHOTS),
      baselineMemory_(0), leakDetectionActive_(false),
      mockCurrentTime_(0), mockFreeMemory_(2048)
{
    currentTestName_[0] = '\0';

    // 스냅샷 배열 초기화
    for (int i = 0; i < MAX_MEMORY_SNAPSHOTS; i++)
    {
        snapshots_[i] = MemorySnapshotFixed();
    }
}

inline void MockMemoryLeakDetector::recordSnapshot(int freeBytes, int usedBytes,
                                                   const char *eventType, const char *description)
{
    if (snapshotCount_ >= maxHistorySize_)
    {
        // 오래된 스냅샷 제거 (순환 버퍼 방식)
        for (int i = 1; i < maxHistorySize_; i++)
        {
            snapshots_[i - 1] = snapshots_[i];
        }
        snapshotCount_ = maxHistorySize_ - 1;
    }

    MemorySnapshotFixed newSnapshot(mockCurrentTime_, freeBytes, usedBytes, eventType, description);
    addSnapshot(newSnapshot);
}

inline void MockMemoryLeakDetector::recordCurrentMemory(const char *eventType, const char *description)
{
    recordSnapshot(mockFreeMemory_, 2048 - mockFreeMemory_, eventType, description);
}

inline void MockMemoryLeakDetector::clearHistory()
{
    snapshotCount_ = 0;
    for (int i = 0; i < MAX_MEMORY_SNAPSHOTS; i++)
    {
        snapshots_[i] = MemorySnapshotFixed();
    }
}

inline void MockMemoryLeakDetector::setMaxHistorySize(int maxSize)
{
    if (maxSize > 0 && maxSize <= MAX_MEMORY_SNAPSHOTS)
    {
        maxHistorySize_ = maxSize;
    }
}

inline void MockMemoryLeakDetector::startLeakDetection(const char *testName)
{
    safeCopyString(currentTestName_, testName, MAX_STRING_LENGTH);
    baselineMemory_ = mockFreeMemory_;
    leakDetectionActive_ = true;

    // 베이스라인 스냅샷 기록
    recordCurrentMemory("LEAK_DETECTION_START", testName);
}

inline void MockMemoryLeakDetector::recordTestIteration(int iteration, int freeMemory)
{
    if (!leakDetectionActive_)
        return;

    mockFreeMemory_ = freeMemory;
    mockCurrentTime_ += 100; // 100ms씩 증가

    char iterDesc[MAX_STRING_LENGTH];
    snprintf(iterDesc, MAX_STRING_LENGTH, "Iteration_%d", iteration);

    recordCurrentMemory("LEAK_TEST_ITERATION", iterDesc);
}

inline MemoryLeakAnalysis MockMemoryLeakDetector::analyzeLeakPattern()
{
    MemoryLeakAnalysis analysis;

    if (snapshotCount_ < 2)
    {
        safeCopyString(analysis.summary, "Insufficient data for analysis", sizeof(analysis.summary));
        return analysis;
    }

    analysis.totalSnapshots = snapshotCount_;
    analysis.maxMemoryDecrease = findMaxMemoryDecrease();
    analysis.averageMemoryTrend = calculateMemoryTrend();

    // 의심스러운 메모리 감소 패턴 탐지
    int consecutiveDecreases = 0;
    for (int i = 1; i < snapshotCount_; i++)
    {
        if (snapshots_[i].freeBytes < snapshots_[i - 1].freeBytes)
        {
            consecutiveDecreases++;
            analysis.suspiciousDecreaseCount++;
        }
        else
        {
            consecutiveDecreases = 0;
        }
    }

    // 누수 판정 로직 (조건을 완화)
    analysis.leakDetected = (analysis.suspiciousDecreaseCount >= 3) &&
                            (analysis.maxMemoryDecrease > 5) &&    // 10에서 5로 완화
                            (analysis.averageMemoryTrend < -0.5f); // -1.0에서 -0.5로 완화

    // 요약 생성
    if (analysis.leakDetected)
    {
        snprintf(analysis.summary, sizeof(analysis.summary),
                 "LEAK DETECTED: %d decreases, max decrease %d bytes, trend %.2f",
                 analysis.suspiciousDecreaseCount, analysis.maxMemoryDecrease, analysis.averageMemoryTrend);
    }
    else
    {
        snprintf(analysis.summary, sizeof(analysis.summary),
                 "No significant leak detected. Trend: %.2f, Max decrease: %d",
                 analysis.averageMemoryTrend, analysis.maxMemoryDecrease);
    }

    return analysis;
}

inline bool MockMemoryLeakDetector::detectMemoryLeak(int thresholdBytes, int minDecreaseCount)
{
    MemoryLeakAnalysis analysis = analyzeLeakPattern();
    return analysis.leakDetected &&
           (analysis.maxMemoryDecrease >= thresholdBytes) &&
           (analysis.suspiciousDecreaseCount >= minDecreaseCount);
}

inline void MockMemoryLeakDetector::simulateMemoryStress(int iterations, int memoryDecreasePerIteration)
{
    startLeakDetection("Memory_Stress_Test");

    for (int i = 0; i < iterations; i++)
    {
        mockFreeMemory_ -= memoryDecreasePerIteration;
        if (mockFreeMemory_ < 100)
            mockFreeMemory_ = 100; // 최소 메모리 보장

        recordTestIteration(i + 1, mockFreeMemory_);
    }
}

inline void MockMemoryLeakDetector::simulateMemoryRecovery(int recoveryBytes)
{
    mockFreeMemory_ += recoveryBytes;
    if (mockFreeMemory_ > 2048)
        mockFreeMemory_ = 2048; // 최대 메모리 제한

    recordCurrentMemory("MEMORY_RECOVERY", "Recovery simulation");
}

inline int MockMemoryLeakDetector::getSnapshotCount() const
{
    return snapshotCount_;
}

inline MemorySnapshotFixed MockMemoryLeakDetector::getSnapshot(int index) const
{
    if (index >= 0 && index < snapshotCount_)
    {
        return snapshots_[index];
    }
    return MemorySnapshotFixed();
}

inline void MockMemoryLeakDetector::generateLeakReport(char *outputBuffer, int bufferSize) const
{
    if (!outputBuffer || bufferSize <= 0)
        return;

    int offset = 0;

    // 헤더
    offset += snprintf(outputBuffer + offset, bufferSize - offset,
                       "=== Memory Leak Detection Report ===\n");
    offset += snprintf(outputBuffer + offset, bufferSize - offset,
                       "Test: %s\n", currentTestName_);
    offset += snprintf(outputBuffer + offset, bufferSize - offset,
                       "Total Snapshots: %d\n", snapshotCount_);
    offset += snprintf(outputBuffer + offset, bufferSize - offset,
                       "Baseline Memory: %d bytes\n\n", baselineMemory_);

    // 스냅샷 데이터 (최근 10개만)
    int startIdx = (snapshotCount_ > 10) ? snapshotCount_ - 10 : 0;
    offset += snprintf(outputBuffer + offset, bufferSize - offset,
                       "Recent Memory Snapshots:\n");

    for (int i = startIdx; i < snapshotCount_ && offset < bufferSize - 100; i++)
    {
        offset += snprintf(outputBuffer + offset, bufferSize - offset,
                           "[%lu] Free: %d, Type: %s, Desc: %s\n",
                           snapshots_[i].timestamp, snapshots_[i].freeBytes,
                           snapshots_[i].eventType, snapshots_[i].description);
    }
}

inline void MockMemoryLeakDetector::setMockCurrentTime(unsigned long mockTime)
{
    mockCurrentTime_ = mockTime;
}

inline void MockMemoryLeakDetector::setMockFreeMemory(int mockFreeMemory)
{
    mockFreeMemory_ = mockFreeMemory;
}

// --- Private 메서드 구현 ---

inline void MockMemoryLeakDetector::addSnapshot(const MemorySnapshotFixed &snapshot)
{
    if (snapshotCount_ < maxHistorySize_)
    {
        snapshots_[snapshotCount_] = snapshot;
        snapshotCount_++;
    }
}

inline float MockMemoryLeakDetector::calculateMemoryTrend() const
{
    if (snapshotCount_ < 2)
        return 0.0f;

    // 단순 선형 추세 계산 (마지막 값과 첫 번째 값의 차이를 시간으로 나눔)
    int firstMemory = snapshots_[0].freeBytes;
    int lastMemory = snapshots_[snapshotCount_ - 1].freeBytes;
    unsigned long timeDiff = snapshots_[snapshotCount_ - 1].timestamp - snapshots_[0].timestamp;

    if (timeDiff == 0)
        return 0.0f;

    return (float)(lastMemory - firstMemory) / (float)timeDiff * 1000.0f; // bytes/second
}

inline int MockMemoryLeakDetector::findMaxMemoryDecrease() const
{
    int maxDecrease = 0;

    for (int i = 1; i < snapshotCount_; i++)
    {
        int decrease = snapshots_[i - 1].freeBytes - snapshots_[i].freeBytes;
        if (decrease > maxDecrease)
        {
            maxDecrease = decrease;
        }
    }

    return maxDecrease;
}

// (클래스 외부 전역 safeCopyString 사용, 중복 정의 제거)
