#ifndef MEMORY_STRESS_HELPER_H
#define MEMORY_STRESS_HELPER_H

#include "../scenario/MemoryStressScenario.h"
#include "../scenario/StressTestResult.h"

namespace MemoryStressHelper
{
    inline void simulateMemoryAllocations(const MemoryStressScenario &scenario, int &currentMemoryBytes, int &peakUsage, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.allocationCount; i++)
        {
            currentMemoryBytes -= scenario.allocationSize;
            if (currentMemoryBytes < peakUsage)
            {
                peakUsage = currentMemoryBytes;
            }
            currentTimeMs += 10;
        }
    }
    inline void simulatePartialDeallocation(const MemoryStressScenario &scenario, int &currentMemoryBytes)
    {
        for (int i = scenario.deallocateAfter; i < scenario.allocationCount; i += 2)
        {
            currentMemoryBytes += scenario.allocationSize / 2;
        }
    }
    inline void simulateFragmentation(const MemoryStressScenario &scenario, int &currentMemoryBytes)
    {
        if (scenario.enableFragmentation)
        {
            currentMemoryBytes -= 100;
        }
    }
    inline void setMemoryPerformanceRating(StressTestResult &result)
    {
        if (result.operationsPerSecond > 100)
        {
            result.performanceRating = "EXCELLENT";
        }
        else if (result.operationsPerSecond > 50)
        {
            result.performanceRating = "GOOD";
        }
        else if (result.operationsPerSecond > 20)
        {
            result.performanceRating = "FAIR";
        }
        else
        {
            result.performanceRating = "POOR";
        }
    }
}

#endif // MEMORY_STRESS_HELPER_H
