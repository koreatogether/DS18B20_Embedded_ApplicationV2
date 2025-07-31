#ifndef IO_STRESS_HELPER_H
#define IO_STRESS_HELPER_H

#include "../scenario/IoStressScenario.h"
#include "../scenario/StressTestResult.h"

namespace IoStressHelper
{
    inline void simulateSerialWrite(const IoStressScenario &scenario, int &operations, int &errors, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.serialWriteOperations; i++)
        {
            operations++;
            currentTimeMs += 5;
            if (scenario.simulateErrors && i % 50 == 0)
            {
                errors++;
            }
        }
    }
    inline void simulateSerialRead(const IoStressScenario &scenario, int &operations, int &errors, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.serialReadOperations; i++)
        {
            operations++;
            currentTimeMs += 8;
            if (scenario.simulateErrors && i % 30 == 0)
            {
                errors++;
            }
        }
    }
    inline void simulateSensorRead(const IoStressScenario &scenario, int &operations, int &errors, int &currentTimeMs)
    {
        for (int i = 0; i < scenario.sensorReadOperations; i++)
        {
            operations++;
            currentTimeMs += 15;
            if (scenario.simulateErrors && i % 20 == 0)
            {
                errors++;
            }
        }
    }
    inline void setIoPerformanceRating(StressTestResult &result, double errorRate)
    {
        if (result.operationsPerSecond > 100 && errorRate < 0.02)
        {
            result.performanceRating = "EXCELLENT";
        }
        else if (result.operationsPerSecond > 50 && errorRate < 0.05)
        {
            result.performanceRating = "GOOD";
        }
        else if (result.operationsPerSecond > 20 && errorRate < 0.10)
        {
            result.performanceRating = "FAIR";
        }
        else
        {
            result.performanceRating = "POOR";
        }
    }
}

#endif // IO_STRESS_HELPER_H
