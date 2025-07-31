#ifndef MEMORY_STRESS_SCENARIO_H
#define MEMORY_STRESS_SCENARIO_H

#include <string>

struct MemoryStressScenario
{
    std::string scenarioName;
    int allocationSize;
    int allocationCount;
    int deallocateAfter;
    int expectedMemoryDrop;
    bool enableFragmentation;

    MemoryStressScenario() : scenarioName(""), allocationSize(0), allocationCount(0),
                             deallocateAfter(0), expectedMemoryDrop(0), enableFragmentation(false) {}
};

#endif // MEMORY_STRESS_SCENARIO_H
