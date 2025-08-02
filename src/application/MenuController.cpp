#include "MenuController.h"
#include "SensorController.h"
#include <OneWire.h>
#include <DallasTemperature.h>

extern SensorController sensorController;
extern unsigned long lastPrint;
extern const unsigned long printInterval;

MenuController::MenuController() : appState(AppState::Normal), selectedSensorIdx(-1), selectedDisplayIdx(-1)
{
}

void MenuController::printMenu()
{
    Serial.println();
    Serial.println("===== 센서 제어 메뉴 =====");
    Serial.println("1. 센서 ID 조정");
    Serial.println("2. 상/하한 온도 조정");
    Serial.println("3. 취소 / 상태창으로 돌아가기");
    Serial.print("메뉴 번호를 입력하세요: ");
}

void MenuController::printSensorIdMenu()
{
    Serial.println();
    Serial.println("--- 센서 ID 조정 메뉴 ---");
    Serial.println("1. 개별 센서 ID 변경");
    Serial.println("2. 복수의 센서 ID 변경");
    Serial.println("3. 주소순 자동 ID 할당");
    Serial.println("4. 전체 ID 초기화");
    Serial.println("5. 이전 메뉴 이동");
    Serial.println("6. 상태창으로 돌아가기");
    Serial.print("메뉴 번호를 입력하세요: ");
}

void MenuController::handleSerialInput()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\r' || c == '\n')
        {
            if (inputBuffer.length() > 0)
            {
                Serial.print("[DEBUG] appState: ");
                Serial.println((int)appState);
                Serial.print("[DEBUG] inputBuffer: ");
                Serial.println(inputBuffer);

                switch (appState)
                {
                case AppState::Normal:
                    handleNormalState();
                    break;
                case AppState::Menu:
                    handleMenuState();
                    break;
                case AppState::SensorIdMenu:
                    handleSensorIdMenuState();
                    break;
                case AppState::SensorIdChange_SelectSensor:
                    handleSensorIdSelectState();
                    break;
                case AppState::SensorIdChange_ConfirmSensor:
                    handleSensorIdConfirmState();
                    break;
                case AppState::SensorIdChange_InputId:
                    handleSensorIdInputState();
                    break;
                }
                inputBuffer = "";
            }
        }
        else if (!isspace(c))
        {
            inputBuffer += c;
        }
    }
}

void MenuController::handleNormalState()
{
    if (inputBuffer == "menu" || inputBuffer == "m")
    {
        appState = AppState::Menu;
        Serial.println("[DEBUG] appState -> Menu");
        printMenu();
    }
}

void MenuController::handleMenuState()
{
    if (inputBuffer == "1")
    {
        appState = AppState::SensorIdMenu;
        Serial.println("[DEBUG] appState -> SensorIdMenu");
        printSensorIdMenu();
    }
    else if (inputBuffer == "2")
    {
        Serial.println("[상/하한 온도 조정 메뉴는 추후 구현]");
        printMenu();
    }
    else if (inputBuffer == "3")
    {
        appState = AppState::Normal;
        Serial.println("[DEBUG] appState -> Normal");
        sensorController.printSensorStatusTable();
        lastPrint = millis();
    }
    else
    {
        Serial.println("지원하지 않는 메뉴입니다. 1~3 중 선택하세요.");
        printMenu();
    }
}

void MenuController::handleSensorIdMenuState()
{
    if (inputBuffer == "1")
    {
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor");
        Serial.println("[개별 센서 ID 변경] 센서 상태창:");
        sensorController.printSensorStatusTable();
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
    }
    else if (inputBuffer == "3")
    {
        sensorController.assignIDsByAddress();
        Serial.println("[자동] 주소순 ID 할당 완료");
        sensorController.printSensorStatusTable();
        printSensorIdMenu();
    }
    else if (inputBuffer == "5")
    {
        appState = AppState::Menu;
        Serial.println("[DEBUG] appState -> Menu");
        printMenu();
    }
    else if (inputBuffer == "6")
    {
        appState = AppState::Normal;
        Serial.println("[DEBUG] appState -> Normal");
        sensorController.printSensorStatusTable();
        lastPrint = millis();
    }
    else
    {
        Serial.println("지원하지 않는 메뉴입니다. 1, 5: 이전, 6: 상태창으로 돌아가기");
        printSensorIdMenu();
    }
}

void MenuController::handleSensorIdSelectState()
{
    if (inputBuffer == "c" || inputBuffer == "C")
    {
        appState = AppState::SensorIdMenu;
        Serial.println("[DEBUG] appState -> SensorIdMenu");
        printSensorIdMenu();
    }
    else
    {
        int sensorIdx = inputBuffer.toInt();
        Serial.print("[DEBUG] sensorIdx: ");
        Serial.println(sensorIdx);
        const auto *sortedRows = sensorController.getSortedSensorRows();
        if (sensorIdx >= 1 && sensorIdx <= SENSOR_MAX_COUNT && sortedRows[sensorIdx - 1].connected)
        {
            selectedDisplayIdx = sensorIdx;
            selectedSensorIdx = sortedRows[sensorIdx - 1].idx;
            Serial.print("[DEBUG] selectedDisplayIdx (번호): ");
            Serial.println(selectedDisplayIdx);
            Serial.print("[DEBUG] selectedSensorIdx (물리 인덱스): ");
            Serial.println(selectedSensorIdx);
            appState = AppState::SensorIdChange_ConfirmSensor;
            Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor");
            Serial.print("센서 ");
            Serial.print(selectedDisplayIdx);
            Serial.println("번을 변경할까요? (y/n, 취소:c)");
        }
        else
        {
            Serial.println("[오류] 연결된 센서만 선택할 수 있습니다. (1~8)");
            Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
        }
    }
}

void MenuController::handleSensorIdConfirmState()
{
    if (inputBuffer == "y" || inputBuffer == "Y")
    {
        appState = AppState::SensorIdChange_InputId;
        Serial.println("[DEBUG] appState -> SensorIdChange_InputId");
        Serial.print("센서 ");
        Serial.print(selectedDisplayIdx);
        Serial.println("의 새로운 ID(1~8, 취소:c)를 입력하세요: ");
    }
    else if (inputBuffer == "n" || inputBuffer == "N")
    {
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor");
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
    }
    else if (inputBuffer == "c" || inputBuffer == "C")
    {
        appState = AppState::SensorIdMenu;
        Serial.println("[DEBUG] appState -> SensorIdMenu");
        printSensorIdMenu();
    }
    else
    {
        Serial.println("y(예), n(아니오), c(취소) 중 하나를 입력하세요.");
        Serial.print("센서 ");
        Serial.print(selectedDisplayIdx);
        Serial.println("번을 변경할까요? (y/n, 취소:c)");
    }
}

void MenuController::handleSensorIdInputState()
{
    if (inputBuffer == "c" || inputBuffer == "C")
    {
        appState = AppState::SensorIdMenu;
        Serial.println("[DEBUG] appState -> SensorIdMenu (취소)");
        printSensorIdMenu();
    }
    else
    {
        int newId = inputBuffer.toInt();
        Serial.print("[DEBUG] 입력된 newId: ");
        Serial.println(newId);
        if (newId >= 1 && newId <= SENSOR_MAX_COUNT)
        {
            if (sensorController.isIdDuplicated(newId, selectedSensorIdx))
            {
                Serial.println("[오류] 이미 사용 중인 ID입니다. 다른 값을 입력하세요 (취소:c)");
            }
            else
            {
                sensorController.setSensorLogicalId(selectedSensorIdx, newId);
                Serial.print("센서 ");
                Serial.print(selectedDisplayIdx);
                Serial.print("의 ID를 ");
                Serial.print(newId);
                Serial.println("(으)로 변경 완료 (센서 EEPROM 저장)");
                delay(30);
                sensorController.printSensorStatusTable();
                appState = AppState::SensorIdMenu;
                Serial.println("[DEBUG] appState -> SensorIdMenu (변경 후)");
                printSensorIdMenu();
            }
        }
        else
        {
            Serial.println("[오류] ID는 1~8 사이의 숫자여야 합니다. 다시 입력하세요 (취소:c)");
        }
    }
}
