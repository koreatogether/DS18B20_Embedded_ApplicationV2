#ifndef CPU_STRESS_SCENARIO_H
#define CPU_STRESS_SCENARIO_H

#include <string>

struct CpuStressScenario
{
    std::string scenarioName;
    int cpuIntensiveLoops;
    int mathCalculations;
    int stringOperations;
    int expectedDurationMs;
    double targetCpuUsage;

    CpuStressScenario() : scenarioName(""), cpuIntensiveLoops(0), mathCalculations(0),
                          stringOperations(0), expectedDurationMs(0), targetCpuUsage(0.0) {}
};

#endif // CPU_STRESS_SCENARIO_H
