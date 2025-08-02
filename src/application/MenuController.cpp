#include "MenuController.h"
#include "SensorController.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <algorithm>

extern SensorController sensorController;
extern unsigned long lastPrint;
extern const unsigned long printInterval;

MenuController::MenuController() : appState(AppState::Normal), selectedSensorIdx(-1), selectedDisplayIdx(-1)
{
}

// 입력 문자열에서 1~8 사이의 숫자만 추출, 중복 제거
std::vector<int> MenuController::parseSensorIndices(const String &input)
{
    std::vector<int> indices;
    bool used[9] = {false}; // 1~8만 사용
    int len = input.length();
    for (int i = 0; i < len; ++i)
    {
        char c = input.charAt(i);
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

#include "MenuController.h"
#include "SensorController.h"
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
        // 메뉴 안내문만 출력, 상태 전환 없음
        printMenu();
        return;
    }
}

void MenuController::handleSensorIdMenuState()
{
    if (inputBuffer == "1")
    {
        isMultiSelectMode = false;
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor");
        Serial.println("[개별 센서 ID 변경] 센서 상태창:");
        sensorController.printSensorStatusTable();
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
    }
    else if (inputBuffer == "2")
    {
        isMultiSelectMode = true;
        Serial.println("[복수 센서 ID 변경] 센서 상태창:");
        sensorController.printSensorStatusTable();
        Serial.print("변경할 센서 번호들을 입력하세요 (예: 1 2 3, 123, 1,2,3,4, 12 34, 1 2345, 취소:c): ");
        appState = AppState::SensorIdChange_SelectSensor;
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
        // 복수 센서 선택 입력 처리
        std::vector<int> indices = MenuController::parseSensorIndices(inputBuffer);
        const auto *sortedRows = sensorController.getSortedSensorRows();
        selectedSensorIndices.clear();
        for (int idx : indices)
        {
            if (idx >= 1 && idx <= SENSOR_MAX_COUNT && sortedRows[idx - 1].connected)
            {
                selectedSensorIndices.push_back(idx);
            }
        }
        if (selectedSensorIndices.empty())
        {
            Serial.println("[오류] 연결된 센서만 선택할 수 있습니다. (1~8)");
            if (isMultiSelectMode)
                Serial.print("변경할 센서 번호들을 입력하세요 (예: 1 2 3, 123, 1,2,3,4, 12 34, 1 2345, 취소:c): ");
            else
                Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
            return;
        }

        // 선택된 센서 번호 안내
        Serial.print("선택된 센서 번호: ");
        for (int idx : selectedSensorIndices)
        {
            Serial.print(idx);
            Serial.print(" ");
        }
        Serial.println();

        // 첫 번째 센서부터 변경 확인 안내문 출력
        selectedDisplayIdx = selectedSensorIndices[0];
        selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
        appState = AppState::SensorIdChange_ConfirmSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor");
        Serial.print("센서 ");
        Serial.print(selectedDisplayIdx);
        Serial.println("번을 변경할까요? (y/n, 취소:c)");
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
        // 단일 선택일 경우 메뉴로 복귀하지 않음
        return;
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
        // 복수 선택일 경우 다음 센서로 진행
        auto it = std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx);
        if (it != selectedSensorIndices.end() && selectedSensorIndices.size() > 1)
        {
            size_t idx = std::distance(selectedSensorIndices.begin(), it);
            if (idx + 1 < selectedSensorIndices.size())
            {
                selectedDisplayIdx = selectedSensorIndices[idx + 1];
                const auto *sortedRows = sensorController.getSortedSensorRows();
                selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
                appState = AppState::SensorIdChange_ConfirmSensor;
                Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor (다음 센서)");
                Serial.print("센서 ");
                Serial.print(selectedDisplayIdx);
                Serial.println("번을 변경할까요? (y/n, 취소:c)");
                return;
            }
        }
        // 단일 선택일 경우 메뉴로 복귀하지 않음 (위에서 return 처리)
        return;
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
                // 복수 선택일 경우 다음 센서로 진행
                auto it = std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx);
                if (it != selectedSensorIndices.end() && selectedSensorIndices.size() > 1)
                {
                    size_t idx = std::distance(selectedSensorIndices.begin(), it);
                    if (idx + 1 < selectedSensorIndices.size())
                    {
                        selectedDisplayIdx = selectedSensorIndices[idx + 1];
                        const auto *sortedRows = sensorController.getSortedSensorRows();
                        selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
                        appState = AppState::SensorIdChange_ConfirmSensor;
                        Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor (다음 센서)");
                        Serial.print("센서 ");
                        Serial.print(selectedDisplayIdx);
                        Serial.println("번을 변경할까요? (y/n, 취소:c)");
                        return;
                    }
                }
                // 마지막 센서까지 완료 또는 단일 선택이면 메뉴로 복귀
                appState = AppState::SensorIdMenu;
                printSensorIdMenu();
            }
        }
        else
        {
            Serial.println("[오류] 1~8 사이의 값을 입력하세요 (취소:c)");
        }
    }
}
