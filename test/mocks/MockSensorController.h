#pragma once
#include <vector>
#include <cstdint>
#include "../../src/domain/SensorStatus.h"

#ifndef SENSOR_MAX_COUNT
#define SENSOR_MAX_COUNT 8
#endif

struct SensorRow
{
    int idx;
    bool connected;
    uint64_t address;
    int logicalId;
    float temperature;

    SensorRow() : idx(-1), connected(false), address(0), logicalId(0), temperature(0.0f) {}
};

// Header-only mock implementation of SensorController for testing
class SensorController
{
public:
    SensorController()
    {
        // Initialize with some mock sensor data
        for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
        {
            sensorRows[i].idx = i;
            sensorRows[i].connected = (i < 4); // First 4 sensors connected
            sensorRows[i].address = 0x12345678 + i;
            sensorRows[i].logicalId = i + 1;
            sensorRows[i].temperature = 20.0f + i * 2.5f;
        }
        sensorCount = 4;
        updateSortedRows();
    }

    void printSensorStatusTable()
    {
        mockOutput += "| 번호 | ID  | 주소           | 온도      | 상한     | 상한상태 | 하한     | 하한상태 | 상태     |\n";
        mockOutput += "|------|-----|----------------|-----------|----------|----------|----------|----------|----------|\n";
        for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
        {
            if (sortedRows[i].connected)
            {
                char line[200];
                snprintf(line, sizeof(line),
                         "| %2d   | %2d  | 0x%010llX | %7.1f°C | %7.1f°C | %-6s   | %7.1f°C | %-6s   | %-6s   |\n",
                         i + 1, sortedRows[i].logicalId, sortedRows[i].address,
                         sortedRows[i].temperature, 30.0f, "정상", 20.0f, "정상", "정상");
                mockOutput += line;
            }
            else
            {
                char line[200];
                snprintf(line, sizeof(line),
                         "| %2d   | N/A | N/A            | N/A       | N/A      | N/A      | N/A      | N/A      | N/A      |\n",
                         i + 1);
                mockOutput += line;
            }
        }
    }

    void assignIDsByAddress()
    {
        for (int i = 0; i < sensorCount; ++i)
        {
            if (sensorRows[i].connected)
            {
                sensorRows[i].logicalId = i + 1;
            }
        }
        updateSortedRows();
        mockOutput += "[자동] 주소순 ID 할당 완료\n";
    }

    bool isIdDuplicated(int newId, int excludeIdx)
    {
        for (int i = 0; i < sensorCount; ++i)
        {
            if (i != excludeIdx && sensorRows[i].connected && sensorRows[i].logicalId == newId)
            {
                return true;
            }
        }
        return false;
    }

    void setSensorLogicalId(int sensorIdx, int newId)
    {
        if (sensorIdx >= 0 && sensorIdx < sensorCount && sensorRows[sensorIdx].connected)
        {
            sensorRows[sensorIdx].logicalId = newId;
            updateSortedRows();
            mockOutput += "센서 EEPROM 저장 완료\n";
        }
    }

    const SensorRow *getSortedSensorRows() const
    {
        return sortedRows;
    }

    int getSensorCount() const { return sensorCount; }

    // Mock output for testing
    std::string getMockOutput() const { return mockOutput; }
    void clearMockOutput() { mockOutput.clear(); }

private:
    SensorRow sensorRows[SENSOR_MAX_COUNT];
    SensorRow sortedRows[SENSOR_MAX_COUNT];
    int sensorCount;
    std::string mockOutput;

    void updateSortedRows()
    {
        // Copy and sort by logical ID
        for (int i = 0; i < SENSOR_MAX_COUNT; ++i)
        {
            sortedRows[i] = sensorRows[i];
        }

        // Simple bubble sort by logical ID
        for (int i = 0; i < SENSOR_MAX_COUNT - 1; ++i)
        {
            for (int j = 0; j < SENSOR_MAX_COUNT - 1 - i; ++j)
            {
                if (sortedRows[j].connected && sortedRows[j + 1].connected)
                {
                    if (sortedRows[j].logicalId > sortedRows[j + 1].logicalId)
                    {
                        SensorRow temp = sortedRows[j];
                        sortedRows[j] = sortedRows[j + 1];
                        sortedRows[j + 1] = temp;
                    }
                }
            }
        }
    }
};
