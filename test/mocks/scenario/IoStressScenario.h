#ifndef IO_STRESS_SCENARIO_H
#define IO_STRESS_SCENARIO_H

#include <string>

struct IoStressScenario
{
    std::string scenarioName;
    int serialWriteOperations;
    int serialReadOperations;
    int sensorReadOperations;
    int expectedLatencyMs;
    bool simulateErrors;

    IoStressScenario() : scenarioName(""), serialWriteOperations(0), serialReadOperations(0),
                         sensorReadOperations(0), expectedLatencyMs(0), simulateErrors(false) {}
};

#endif // IO_STRESS_SCENARIO_H
