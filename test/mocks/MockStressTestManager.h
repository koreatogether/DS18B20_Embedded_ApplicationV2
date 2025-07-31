// ...existing code...
inline bool isTestPassed(int durationMs, int expectedDurationMs)
{
    int timeTolerance = expectedDurationMs * 20 / 100;
    return (durationMs <= (expectedDurationMs + timeTolerance));
}
#ifndef MOCK_STRESS_TEST_MANAGER_H
#define MOCK_STRESS_TEST_MANAGER_H

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

#include "scenario/StressTestResult.h"
#include "scenario/MemoryStressScenario.h"
#include "scenario/CpuStressScenario.h"
#include "scenario/IoStressScenario.h"
#include "utils/MemoryStressHelper.h"
#include "utils/CpuStressHelper.h"
#include "utils/IoStressHelper.h"
#include "utils/SystemStabilityHelper.h"

class MockStressTestManager
{
private:
    std::vector<StressTestResult> testResults;
    int currentMemoryBytes;
    int initialMemoryBytes;
    int currentTimeMs;
    bool testingEnabled;

    // 고정 크기 배열로 시나리오 저장 (STL 없이)
    static const int MAX_SCENARIOS = 10;
    MemoryStressScenario memoryScenarios[MAX_SCENARIOS];
    CpuStressScenario cpuScenarios[MAX_SCENARIOS];
    IoStressScenario ioScenarios[MAX_SCENARIOS];
    int memoryScenarioCount;
    int cpuScenarioCount;
    int ioScenarioCount;

public:
    inline MockStressTestManager() : currentMemoryBytes(2048), initialMemoryBytes(2048),
                                     currentTimeMs(0), testingEnabled(true),
                                     memoryScenarioCount(0), cpuScenarioCount(0), ioScenarioCount(0)
    {

        // 기본 메모리 스트레스 시나리오 초기화
        addMemoryStressScenario("Small_Allocations", 32, 50, 25, 1600, false);
        addMemoryStressScenario("Large_Allocations", 256, 8, 4, 2048, false);
        addMemoryStressScenario("Fragmentation_Test", 64, 30, 15, 1920, true);

        // 기본 CPU 스트레스 시나리오 초기화
        addCpuStressScenario("Math_Intensive", 10000, 5000, 1000, 2000, 80.0);
        addCpuStressScenario("String_Processing", 5000, 2000, 8000, 3000, 70.0);
        addCpuStressScenario("Mixed_Operations", 7500, 3500, 4500, 2500, 85.0);

        // 기본 I/O 스트레스 시나리오 초기화
        addIoStressScenario("Serial_Flood", 1000, 500, 0, 5000, false);
        addIoStressScenario("Sensor_Burst", 0, 0, 200, 3000, false);
        addIoStressScenario("Error_Simulation", 300, 150, 50, 4000, true);
    }

    // 메모리 스트레스 시나리오 추가
    inline void addMemoryStressScenario(const std::string &name, int allocSize,
                                        int allocCount, int deallocAfter,
                                        int expectedDrop, bool fragmentation)
    {
        if (memoryScenarioCount < MAX_SCENARIOS)
        {
            MemoryStressScenario &scenario = memoryScenarios[memoryScenarioCount++];
            scenario.scenarioName = name;
            scenario.allocationSize = allocSize;
            scenario.allocationCount = allocCount;
            scenario.deallocateAfter = deallocAfter;
            scenario.expectedMemoryDrop = expectedDrop;
            scenario.enableFragmentation = fragmentation;
        }
    }

    // CPU 스트레스 시나리오 추가
    inline void addCpuStressScenario(const std::string &name, int loops,
                                     int mathCalcs, int stringOps,
                                     int expectedDuration, double targetCpu)
    {
        if (cpuScenarioCount < MAX_SCENARIOS)
        {
            CpuStressScenario &scenario = cpuScenarios[cpuScenarioCount++];
            scenario.scenarioName = name;
            scenario.cpuIntensiveLoops = loops;
            scenario.mathCalculations = mathCalcs;
            scenario.stringOperations = stringOps;
            scenario.expectedDurationMs = expectedDuration;
            scenario.targetCpuUsage = targetCpu;
        }
    }

    // I/O 스트레스 시나리오 추가
    inline void addIoStressScenario(const std::string &name, int writes,
                                    int reads, int sensorReads,
                                    int expectedLatency, bool errors)
    {
        if (ioScenarioCount < MAX_SCENARIOS)
        {
            IoStressScenario &scenario = ioScenarios[ioScenarioCount++];
            scenario.scenarioName = name;
            scenario.serialWriteOperations = writes;
            scenario.serialReadOperations = reads;
            scenario.sensorReadOperations = sensorReads;
            scenario.expectedLatencyMs = expectedLatency;
            scenario.simulateErrors = errors;
        }
    }

    // 메모리 스트레스 테스트 실행
    inline StressTestResult runMemoryStressTest(int scenarioIndex)
    {
        StressTestResult result;

        if (scenarioIndex < 0 || scenarioIndex >= memoryScenarioCount)
        {
            result.testName = "Invalid_Memory_Scenario";
            result.testPassed = false;
            return result;
        }

        const MemoryStressScenario &scenario = memoryScenarios[scenarioIndex];
        result.testName = "Memory_" + scenario.scenarioName;

        int startTime = currentTimeMs;
        int startMemory = currentMemoryBytes;
        int peakUsage = currentMemoryBytes;

        MemoryStressHelper::simulateMemoryAllocations(scenario, currentMemoryBytes, peakUsage, currentTimeMs);
        MemoryStressHelper::simulatePartialDeallocation(scenario, currentMemoryBytes);
        MemoryStressHelper::simulateFragmentation(scenario, currentMemoryBytes);

        result.durationMs = currentTimeMs - startTime;
        result.operationsPerformed = scenario.allocationCount;
        result.memoryUsedBytes = startMemory - currentMemoryBytes;
        result.peakMemoryUsage = startMemory - peakUsage;
        result.errorCount = (currentMemoryBytes < 100) ? 1 : 0; // 메모리 부족 시 에러

        // 성능 메트릭 계산
        result.operationsPerSecond = (result.durationMs > 0) ? (result.operationsPerformed * 1000.0) / result.durationMs : 0.0;
        result.memoryEfficiency = (result.memoryUsedBytes > 0) ? (double)result.operationsPerformed / result.memoryUsedBytes : 0.0;

        // 테스트 통과 조건 확인
        result.testPassed = (result.memoryUsedBytes <= scenario.expectedMemoryDrop) &&
                            (result.errorCount == 0);

        // 성능 등급 결정
        MemoryStressHelper::setMemoryPerformanceRating(result);

        testResults.push_back(result);
        return result;
    }

    // CPU 스트레스 테스트 실행
    inline StressTestResult runCpuStressTest(int scenarioIndex)
    {
        StressTestResult result;
        if (scenarioIndex < 0 || scenarioIndex >= cpuScenarioCount)
        {
            result.testName = "Invalid_CPU_Scenario";
            result.testPassed = false;
            return result;
        }
        const CpuStressScenario &scenario = cpuScenarios[scenarioIndex];
        result.testName = "CPU_" + scenario.scenarioName;
        int startTime = currentTimeMs;
        int operations = 0;
        CpuStressHelper::simulateCpuLoops(scenario, operations, currentTimeMs);
        CpuStressHelper::simulateMathCalculations(scenario, operations, currentTimeMs);
        CpuStressHelper::simulateStringOperations(scenario, operations, currentTimeMs);
        result.durationMs = currentTimeMs - startTime;
        result.operationsPerformed = operations;
        result.memoryUsedBytes = operations / 10;
        result.peakMemoryUsage = result.memoryUsedBytes;
        result.errorCount = 0;
        result.operationsPerSecond = (result.durationMs > 0) ? (result.operationsPerformed * 1000.0) / result.durationMs : 0.0;
        result.memoryEfficiency = (result.memoryUsedBytes > 0) ? (double)result.operationsPerformed / result.memoryUsedBytes : 100.0;
        result.testPassed = isTestPassed(result.durationMs, scenario.expectedDurationMs);
        CpuStressHelper::setPerformanceRating(result);
        testResults.push_back(result);
        return result;
    }

    // I/O 스트레스 테스트 실행
    inline StressTestResult runIoStressTest(int scenarioIndex)
    {
        StressTestResult result;
        if (scenarioIndex < 0 || scenarioIndex >= ioScenarioCount)
        {
            result.testName = "Invalid_IO_Scenario";
            result.testPassed = false;
            return result;
        }
        const IoStressScenario &scenario = ioScenarios[scenarioIndex];
        result.testName = "IO_" + scenario.scenarioName;
        int startTime = currentTimeMs;
        int operations = 0;
        int errors = 0;
        IoStressHelper::simulateSerialWrite(scenario, operations, errors, currentTimeMs);
        IoStressHelper::simulateSerialRead(scenario, operations, errors, currentTimeMs);
        IoStressHelper::simulateSensorRead(scenario, operations, errors, currentTimeMs);
        result.durationMs = currentTimeMs - startTime;
        result.operationsPerformed = operations;
        result.memoryUsedBytes = operations / 5;
        result.peakMemoryUsage = result.memoryUsedBytes;
        result.errorCount = errors;
        result.operationsPerSecond = (result.durationMs > 0) ? (result.operationsPerformed * 1000.0) / result.durationMs : 0.0;
        result.memoryEfficiency = (result.memoryUsedBytes > 0) ? (double)result.operationsPerformed / result.memoryUsedBytes : 100.0;
        int latencyTolerance = scenario.expectedLatencyMs * 30 / 100;
        double errorRate = (operations > 0) ? (double)errors / operations : 0.0;
        result.testPassed = (result.durationMs <= (scenario.expectedLatencyMs + latencyTolerance)) && (errorRate <= 0.1);
        IoStressHelper::setIoPerformanceRating(result, errorRate);
        testResults.push_back(result);
        return result;
    }

    // 종합 스트레스 테스트 실행
    inline std::string runComprehensiveStressTest()
    {
        if (!testingEnabled)
        {
            return "Stress testing is disabled";
        }

        testResults.clear();
        currentMemoryBytes = initialMemoryBytes;
        currentTimeMs = 0;

        // 모든 메모리 시나리오 실행
        for (int i = 0; i < memoryScenarioCount; i++)
        {
            runMemoryStressTest(i);
        }

        // 모든 CPU 시나리오 실행
        for (int i = 0; i < cpuScenarioCount; i++)
        {
            runCpuStressTest(i);
        }

        // 모든 I/O 시나리오 실행
        for (int i = 0; i < ioScenarioCount; i++)
        {
            runIoStressTest(i);
        }

        return generateComprehensiveReport();
    }

    // 종합 리포트 생성
    // 헬퍼 함수: 통계 요약 생성
    std::string generateSummary(int totalTests, int passedTests, int totalOperations, int totalMemoryUsed, int totalErrors, double totalDuration) const
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer),
                 "Summary:\n"
                 "- Total Tests: %d\n"
                 "- Passed: %d (%.1f%%)\n"
                 "- Failed: %d\n"
                 "- Total Operations: %d\n"
                 "- Total Memory Used: %d bytes\n"
                 "- Total Errors: %d\n"
                 "- Total Duration: %.0f ms\n"
                 "- Overall Ops/sec: %.2f\n\n",
                 totalTests, passedTests,
                 (totalTests > 0) ? (passedTests * 100.0 / totalTests) : 0.0,
                 totalTests - passedTests, totalOperations, totalMemoryUsed,
                 totalErrors, totalDuration,
                 (totalDuration > 0) ? (totalOperations * 1000.0 / totalDuration) : 0.0);
        return std::string(buffer);
    }

    // 헬퍼 함수: 개별 테스트 결과 생성
    std::string generateIndividualResults() const
    {
        std::string resultStr;
        char buffer[256];
        for (const auto &result : testResults)
        {
            snprintf(buffer, sizeof(buffer),
                     "- %s: %s (%.0f ops/sec, %s)\n",
                     result.testName.c_str(),
                     result.testPassed ? "PASS" : "FAIL",
                     result.operationsPerSecond,
                     result.performanceRating.c_str());
            resultStr += buffer;
        }
        return resultStr;
    }

    // 헬퍼 함수: 시스템 건강성 평가
    std::string generateSystemHealth(double passRate, double errorRate) const
    {
        if (passRate >= 90 && errorRate <= 2)
            return "Status: EXCELLENT - System handles stress very well\n";
        else if (passRate >= 75 && errorRate <= 5)
            return "Status: GOOD - System performs adequately under stress\n";
        else if (passRate >= 50 && errorRate <= 10)
            return "Status: FAIR - System shows some stress symptoms\n";
        else
            return "Status: POOR - System struggles under stress\n";
    }

    inline std::string generateComprehensiveReport()
    {
        std::string report = "=== COMPREHENSIVE STRESS TEST REPORT ===\n";
        int totalTests = testResults.size();
        int passedTests = 0, totalOperations = 0, totalMemoryUsed = 0, totalErrors = 0;
        double totalDuration = 0;
        for (const auto &result : testResults)
        {
            if (result.testPassed)
                passedTests++;
            totalOperations += result.operationsPerformed;
            totalMemoryUsed += result.memoryUsedBytes;
            totalErrors += result.errorCount;
            totalDuration += result.durationMs;
        }
        report += generateSummary(totalTests, passedTests, totalOperations, totalMemoryUsed, totalErrors, totalDuration);
        report += "Individual Test Results:\n";
        report += generateIndividualResults();
        double passRate = (totalTests > 0) ? (passedTests * 100.0 / totalTests) : 0.0;
        double errorRate = (totalOperations > 0) ? (totalErrors * 100.0 / totalOperations) : 0.0;
        report += "\nSystem Health Assessment:\n";
        report += generateSystemHealth(passRate, errorRate);
        return report;
    }

    // 특정 부하 수준에서 시스템 안정성 테스트
    // --- Helper functions for System Stability Test ---
    // System stability helpers moved to SystemStabilityHelper.h
    // ...existing code...
    inline StressTestResult runSystemStabilityTest(int loadLevel)
    {
        StressTestResult result;
        result.testName = "System_Stability_Load_" + std::to_string(loadLevel);

        int startTime = currentTimeMs;
        int operations = 0;
        int errors = 0;

        int baseOperations = 1000;
        int totalOperations = baseOperations * loadLevel;

        for (int i = 0; i < totalOperations; i++)
        {
            operations++;
            SystemStabilityHelper::simulateMemoryPressure(currentMemoryBytes, i, errors);
            SystemStabilityHelper::simulateCpuLoad(currentTimeMs, loadLevel);
            SystemStabilityHelper::simulateMemoryRecovery(currentMemoryBytes, i);
        }

        result.durationMs = currentTimeMs - startTime;
        result.operationsPerformed = operations;
        result.memoryUsedBytes = initialMemoryBytes - currentMemoryBytes;
        result.peakMemoryUsage = result.memoryUsedBytes;
        result.errorCount = errors;

        result.operationsPerSecond = (result.durationMs > 0) ? (operations * 1000.0) / result.durationMs : 0.0;
        result.memoryEfficiency = (result.memoryUsedBytes > 0) ? (double)operations / result.memoryUsedBytes : 100.0;

        double errorRate = (operations > 0) ? (double)errors / operations : 0.0;
        result.testPassed = (errorRate <= 0.05) && (currentMemoryBytes > 200);
        SystemStabilityHelper::setSystemStabilityPerformanceRating(result, errorRate);
        testResults.push_back(result);
        return result;
    }

    // 테스트 설정 및 상태 관리
    inline void setCurrentMemory(int bytes) { currentMemoryBytes = bytes; }
    inline void setCurrentTime(int timeMs) { currentTimeMs = timeMs; }
    inline void enableTesting(bool enabled) { testingEnabled = enabled; }
    inline void resetToInitialState()
    {
        currentMemoryBytes = initialMemoryBytes;
        currentTimeMs = 0;
        testResults.clear();
    }

    // 상태 조회
    inline int getCurrentMemory() const { return currentMemoryBytes; }
    inline int getCurrentTime() const { return currentTimeMs; }
    inline bool isTestingEnabled() const { return testingEnabled; }
    inline int getTestResultCount() const { return testResults.size(); }
    inline const std::vector<StressTestResult> &getTestResults() const { return testResults; }

    // 시나리오 개수 조회
    inline int getMemoryScenarioCount() const { return memoryScenarioCount; }
    inline int getCpuScenarioCount() const { return cpuScenarioCount; }
    inline int getIoScenarioCount() const { return ioScenarioCount; }
};

#endif // MOCK_STRESS_TEST_MANAGER_H
