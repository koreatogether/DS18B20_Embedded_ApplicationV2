#ifndef MOCK_LONG_RUN_STABILITY_MANAGER_H
#define MOCK_LONG_RUN_STABILITY_MANAGER_H

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

struct LongRunStabilityResult
{
    std::string testName;
    int totalDurationSec;
    int totalIterations;
    int errorCount;
    int memoryLeakCount;
    int minFreeMemory;
    int maxFreeMemory;
    int avgFreeMemory;
    bool passed;
    std::string summary;
    LongRunStabilityResult() : testName(""), totalDurationSec(0), totalIterations(0), errorCount(0), memoryLeakCount(0), minFreeMemory(0), maxFreeMemory(0), avgFreeMemory(0), passed(false), summary("") {}
};

class MockLongRunStabilityManager
{
private:
    static const int MAX_ITER = 3600; // 최대 1시간(1초 단위)
    int freeMemoryHistory[MAX_ITER];
    int errorHistory[MAX_ITER];
    int memoryLeakHistory[MAX_ITER];
    int iterCount;
    int minFreeMemory;
    int maxFreeMemory;
    int sumFreeMemory;
    int errorCount;
    int memoryLeakCount;
    int lastFreeMemory;
    int leakThreshold;
    bool running;

public:
    inline MockLongRunStabilityManager() : iterCount(0), minFreeMemory(999999), maxFreeMemory(0), sumFreeMemory(0), errorCount(0), memoryLeakCount(0), lastFreeMemory(2048), leakThreshold(16), running(false)
    {
        memset(freeMemoryHistory, 0, sizeof(freeMemoryHistory));
        memset(errorHistory, 0, sizeof(errorHistory));
        memset(memoryLeakHistory, 0, sizeof(memoryLeakHistory));
    }
    inline void start(int initialFreeMemory = 2048, int leakThresholdBytes = 16)
    {
        iterCount = 0;
        minFreeMemory = 999999;
        maxFreeMemory = 0;
        sumFreeMemory = 0;
        errorCount = 0;
        memoryLeakCount = 0;
        lastFreeMemory = initialFreeMemory;
        leakThreshold = leakThresholdBytes;
        running = true;
    }
    inline void stop() { running = false; }
    inline void simulateIteration(int freeMemory, bool error = false)
    {
        if (!running || iterCount >= MAX_ITER)
            return;
        freeMemoryHistory[iterCount] = freeMemory;
        errorHistory[iterCount] = error ? 1 : 0;
        if (freeMemory < minFreeMemory)
            minFreeMemory = freeMemory;
        if (freeMemory > maxFreeMemory)
            maxFreeMemory = freeMemory;
        sumFreeMemory += freeMemory;
        if (error)
            errorCount++;
        // 메모리 누수 감지(이전보다 leakThreshold 이상 감소)
        if (lastFreeMemory - freeMemory >= leakThreshold)
        {
            memoryLeakHistory[iterCount] = 1;
            memoryLeakCount++;
        }
        else
        {
            memoryLeakHistory[iterCount] = 0;
        }
        lastFreeMemory = freeMemory;
        iterCount++;
    }
    inline LongRunStabilityResult analyze()
    {
        LongRunStabilityResult result;
        result.testName = "LongRunStabilityTest";
        result.totalDurationSec = iterCount;
        result.totalIterations = iterCount;
        result.errorCount = errorCount;
        result.memoryLeakCount = memoryLeakCount;
        result.minFreeMemory = minFreeMemory;
        result.maxFreeMemory = maxFreeMemory;
        result.avgFreeMemory = (iterCount > 0) ? (sumFreeMemory / iterCount) : 0;
        result.passed = (errorCount == 0 && memoryLeakCount == 0);
        char buf[256];
        snprintf(buf, sizeof(buf), "Duration: %ds, Errors: %d, Leaks: %d, Min: %d, Max: %d, Avg: %d", result.totalDurationSec, errorCount, memoryLeakCount, minFreeMemory, maxFreeMemory, result.avgFreeMemory);
        result.summary = buf;
        return result;
    }
    // 상태 조회
    inline int getIterationCount() const { return iterCount; }
    inline int getErrorCount() const { return errorCount; }
    inline int getMemoryLeakCount() const { return memoryLeakCount; }
    inline int getMinFreeMemory() const { return minFreeMemory; }
    inline int getMaxFreeMemory() const { return maxFreeMemory; }
    inline int getAvgFreeMemory() const { return (iterCount > 0) ? (sumFreeMemory / iterCount) : 0; }
    inline bool isRunning() const { return running; }
};

#endif // MOCK_LONG_RUN_STABILITY_MANAGER_H
