#pragma once
#include <stdint.h>

struct SensorStatus
{
    uint8_t id;
    uint64_t address;
    float temperature;
    float upperThreshold;
    const char *upperState;
    float lowerThreshold;
    const char *lowerState;
    const char *status;
};
