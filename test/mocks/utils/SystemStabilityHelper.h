#ifndef SYSTEM_STABILITY_HELPER_H
#define SYSTEM_STABILITY_HELPER_H

#include "../scenario/StressTestResult.h"

namespace SystemStabilityHelper
{
    inline void simulateMemoryPressure(int &currentMemoryBytes, int i, int &errors)
    {
        if (i % 100 == 0)
        {
            currentMemoryBytes -= 10;
            if (currentMemoryBytes < 100)
            {
                errors++;
                currentMemoryBytes = 100;
            }
        }
    }
    inline void simulateCpuLoad(int &currentTimeMs, int loadLevel)
    {
        currentTimeMs += (loadLevel > 5) ? 2 : 1;
    }
    inline void simulateMemoryRecovery(int &currentMemoryBytes, int i)
    {
        if (i % 500 == 0)
        {
            currentMemoryBytes += 50;
        }
    }
    inline void setSystemStabilityPerformanceRating(StressTestResult &result, double errorRate)
    {
        if (errorRate < 0.01 && result.operationsPerSecond > 100)
        {
            result.performanceRating = "EXCELLENT";
        }
        else if (errorRate < 0.03 && result.operationsPerSecond > 50)
        {
            result.performanceRating = "GOOD";
        }
        else if (errorRate < 0.05 && result.operationsPerSecond > 20)
        {
            result.performanceRating = "FAIR";
        }
        else
        {
            result.performanceRating = "POOR";
        }
    }
}

#endif // SYSTEM_STABILITY_HELPER_H
