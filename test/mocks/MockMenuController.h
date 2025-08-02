#pragma once
#include <vector>
#include <string>
#include "Arduino.h"

// AppState enum matching firmware
enum class AppState
{
    Normal,
    Menu,
    SensorIdMenu,
    SensorIdChange_SelectSensor,
    SensorIdChange_ConfirmSensor,
    SensorIdChange_InputId,
    SensorIdChange_ConfirmId,
    SensorIdChange_Apply,
};

// Header-only mock implementation of MenuController for testing
class MenuController
{
public:
    MenuController() : appState(AppState::Normal), selectedSensorIdx(-1), selectedDisplayIdx(-1), isMultiSelectMode(false) {}

    // Static method for parsing sensor indices - exactly matching firmware implementation
    static std::vector<int> parseSensorIndices(const std::string &input)
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

    // Mock methods - header-only inline implementations with state tracking
    void printMenu()
    {
        mockOutput += "===== 센서 제어 메뉴 =====\n";
        mockOutput += "1. 센서 ID 조정\n";
        mockOutput += "2. 상/하한 온도 조정\n";
        mockOutput += "3. 취소 / 상태창으로 돌아가기\n";
        mockOutput += "메뉴 번호를 입력하세요: ";
    }

    void printSensorIdMenu()
    {
        mockOutput += "--- 센서 ID 조정 메뉴 ---\n";
        mockOutput += "1. 개별 센서 ID 변경\n";
        mockOutput += "2. 복수의 센서 ID 변경\n";
        mockOutput += "3. 주소순 자동 ID 할당\n";
        mockOutput += "4. 전체 ID 초기화\n";
        mockOutput += "5. 이전 메뉴 이동\n";
        mockOutput += "6. 상태창으로 돌아가기\n";
        mockOutput += "메뉴 번호를 입력하세요: ";
    }

    void handleSerialInput() { /* mock - could simulate input processing */ }

    // State accessors for testing
    AppState getAppState() const { return appState; }
    void setAppState(AppState state) { appState = state; }
    int getSelectedSensorIdx() const { return selectedSensorIdx; }
    int getSelectedDisplayIdx() const { return selectedDisplayIdx; }
    const std::vector<int> &getSelectedSensorIndices() const { return selectedSensorIndices; }
    bool getIsMultiSelectMode() const { return isMultiSelectMode; }

    // Mock output for testing
    std::string getMockOutput() const { return mockOutput; }
    void clearMockOutput() { mockOutput.clear(); }

    // Simulate state transitions for testing
    void simulateMenuInput(const std::string &input)
    {
        if (appState == AppState::Menu)
        {
            if (input == "1")
            {
                appState = AppState::SensorIdMenu;
            }
            else if (input == "3")
            {
                appState = AppState::Normal;
            }
        }
        else if (appState == AppState::SensorIdMenu)
        {
            if (input == "1")
            {
                isMultiSelectMode = false;
                appState = AppState::SensorIdChange_SelectSensor;
            }
            else if (input == "2")
            {
                isMultiSelectMode = true;
                appState = AppState::SensorIdChange_SelectSensor;
            }
            else if (input == "5")
            {
                appState = AppState::Menu;
            }
            else if (input == "6")
            {
                appState = AppState::Normal;
            }
        }
    }

    void simulateSensorSelection(const std::string &input)
    {
        if (appState == AppState::SensorIdChange_SelectSensor)
        {
            if (input != "c" && input != "C")
            {
                selectedSensorIndices = parseSensorIndices(input);
                if (!selectedSensorIndices.empty())
                {
                    selectedDisplayIdx = selectedSensorIndices[0];
                    selectedSensorIdx = selectedDisplayIdx - 1; // Mock mapping
                    appState = AppState::SensorIdChange_ConfirmSensor;
                }
            }
        }
    }

private:
    std::vector<int> selectedSensorIndices;
    AppState appState;
    std::string inputBuffer;
    int selectedSensorIdx;
    int selectedDisplayIdx;
    bool isMultiSelectMode;
    std::string mockOutput;
};
