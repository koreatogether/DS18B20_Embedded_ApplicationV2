#pragma once
#include <Arduino.h>

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

class MenuController
{
public:
    MenuController();

    void printMenu();
    void printSensorIdMenu();
    void handleSerialInput();

    AppState getAppState() const { return appState; }
    void setAppState(AppState state) { appState = state; }

    int getSelectedSensorIdx() const { return selectedSensorIdx; }
    int getSelectedDisplayIdx() const { return selectedDisplayIdx; }

private:
    AppState appState;
    String inputBuffer;
    int selectedSensorIdx;
    int selectedDisplayIdx;

    void handleNormalState();
    void handleMenuState();
    void handleSensorIdMenuState();
    void handleSensorIdSelectState();
    void handleSensorIdConfirmState();
    void handleSensorIdInputState();
};
