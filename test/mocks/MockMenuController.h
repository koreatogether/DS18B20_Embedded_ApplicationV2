#pragma once
#include <vector>
#include <string>
#include "Arduino.h"

// Header-only mock implementation of MenuController for testing
class MenuController
{
public:
    MenuController() = default;

    // Static method for parsing sensor indices - inline implementation
    static std::vector<int> parseSensorIndices(const String &input)
    {
        std::vector<int> indices;
        bool used[9] = {false}; // 1~8만 사용
        int len = input.length();
        for (int i = 0; i < len; ++i)
        {
            char c = input[i];
            if (c >= '1' && c <= '8')
            {
                int idx = c - '0';
                if (!used[idx])
                {
                    indices.push_back(idx);
                    used[idx] = true;
                }
            }
        }
        return indices;
    }

    // Mock methods - header-only inline implementations
    void printMenu() { /* mock */ }
    void printSensorIdMenu() { /* mock */ }
    void handleSerialInput() { /* mock */ }

private:
    std::vector<int> selectedSensorIndices;
    int selectedSensorIdx = -1;
    int selectedDisplayIdx = -1;
};
