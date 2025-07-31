#pragma once
#include "../../src/domain/ITemperatureSensor.h"
#include <vector>
#include <cstdint>

class MockTemperatureSensor : public ITemperatureSensor
{
public:
    struct Entry
    {
        uint64_t address;
        float temperature;
    };
    std::vector<Entry> entries;
    bool beginCalled = false;

    MockTemperatureSensor(const std::vector<Entry> &data) : entries(data) {}
    bool begin() override
    {
        beginCalled = true;
        return true;
    }
    int getSensorCount() override { return static_cast<int>(entries.size()); }
    bool getAddress(uint8_t index, uint64_t &address) override
    {
        if (index >= entries.size())
            return false;
        address = entries[index].address;
        return true;
    }
    float getTemperature(uint8_t index) override
    {
        if (index >= entries.size())
            return 0.0f;
        return entries[index].temperature;
    }
};
