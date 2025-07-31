#ifndef CPU_STRESS_HELPER_H
#define CPU_STRESS_HELPER_H

#include "../scenario/CpuStressScenario.h"
#include "../scenario/StressTestResult.h"

namespace CpuStressHelper
{
    inline void simulateCpuLoops(const CpuStressScenario &scenario, int &operations, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.cpuIntensiveLoops; i++)
        {
            operations++;
            currentTimeMs += 1;
        }
    }
    inline void simulateMathCalculations(const CpuStressScenario &scenario, int &operations, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.mathCalculations; i++)
        {
            operations++;
            currentTimeMs += 2;
        }
    }
    inline void simulateStringOperations(const CpuStressScenario &scenario, int &operations, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.stringOperations; i++)
        {
            operations++;
            currentTimeMs += 3;
        }
    }
    inline void setPerformanceRating(StressTestResult &result)
    {
        if (result.operationsPerSecond > 1000)
        {
            result.performanceRating = "EXCELLENT";
        }
        else if (result.operationsPerSecond > 500)
        {
            result.performanceRating = "GOOD";
        }
        else if (result.operationsPerSecond > 200)
        {
            result.performanceRating = "FAIR";
        }
        else
        {
            result.performanceRating = "POOR";
        }
    }
}

#endif // CPU_STRESS_HELPER_H
