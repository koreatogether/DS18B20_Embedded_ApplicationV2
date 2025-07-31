#ifndef STRESS_TEST_RESULT_H
#define STRESS_TEST_RESULT_H

#include <string>

struct StressTestResult
{
    std::string testName;
    int durationMs;
    int operationsPerformed;
    int memoryUsedBytes;
    int peakMemoryUsage;
    int errorCount;
    bool testPassed;
    double operationsPerSecond;
    double memoryEfficiency;
    std::string performanceRating;

    StressTestResult() : testName(""), durationMs(0), operationsPerformed(0),
                         memoryUsedBytes(0), peakMemoryUsage(0), errorCount(0),
                         testPassed(false), operationsPerSecond(0.0),
                         memoryEfficiency(0.0), performanceRating("UNKNOWN") {}
};

#endif // STRESS_TEST_RESULT_H
