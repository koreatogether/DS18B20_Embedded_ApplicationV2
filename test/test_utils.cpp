#include "test_utils.h"
#include <cstdio>
#include <vector>
#include <string>

std::vector<MockTemperatureSensor::Entry> getMockData()
{
    return {
        {0x01234567890, 25.5f},
        {0x01234567891, 29.1f},
        {0x01234567892, 31.2f},
        {0x01234567893, 19.5f},
        {0x01234567894, 22.0f},
        {0x01234567895, 27.8f},
        {0x01234567896, 24.3f},
        {0x01234567897, 26.7f},
    };
}

std::string makeTableLine(int idx, const MockTemperatureSensor::Entry &entry)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "%2d   | %2d  | 0x%010llX | %7.1f°C | %7.1f°C | %-6s   | %7.1f°C | %-6s   | %-6s",
             idx + 1, idx + 1, static_cast<unsigned long long>(entry.address), 
             entry.temperature, 30.0f, "정상", 20.0f, "정상", "정상");
    return std::string(buf);
}
