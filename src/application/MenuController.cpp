#include "MenuController.h"
#include "SensorController.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <algorithm>

extern SensorController sensorController;
extern unsigned long lastPrint;
extern const unsigned long printInterval;

MenuController::MenuController() 
    : appState(AppState::Normal), 
      selectedSensorIdx(-1), 
      selectedDisplayIdx(-1),
      inputBuffer(""),
      isMultiSelectMode(false)
{
    // selectedSensorIndices는 기본 생성자로 빈 벡터로 초기화됨
    selectedSensorIndices.clear(); // 명시적으로 비우기 (선택사항)
}

// SensorMenuHandler를 사용하여 복잡도 감소 (static 함수)
std::vector<int> MenuController::parseSensorIndices(const String &input)
{
    return SensorMenuHandler::parseSensorIndices(input);
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
    // SensorMenuHandler를 사용하여 복잡도 감소
    sensorMenuHandler.printSensorIdMenu();
}

void MenuController::handleSerialInput()
{
    // InputHandler를 사용하여 복잡도 감소
    if (inputHandler.processSerialInput(inputBuffer)) {
        processInputBuffer();
    }
}

void MenuController::resetToNormalState()
{
    appState = AppState::Normal;
    inputBuffer = "";
    selectedSensorIdx = -1;
    selectedDisplayIdx = -1;
    selectedSensorIndices.clear();
    isMultiSelectMode = false;
    Serial.println("[DEBUG] 상태가 Normal로 완전히 리셋되었습니다.");
    Serial.println("[시스템 준비 완료 - Normal 모드에서 대기 중]");
}

void MenuController::handleNormalState()
{
    if (inputBuffer == "menu" || inputBuffer == "m")
    {
        appState = AppState::Menu;
        Serial.println("[DEBUG] appState -> Menu");
        printMenu();
    }
    else if (inputBuffer == "reset" || inputBuffer == "r")
    {
        // 강제 리셋 명령어 추가
        resetToNormalState();
        sensorController.printSensorStatusTable();
        lastPrint = millis();
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
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor (복수)");
        Serial.println("[복수 센서 ID 변경] 센서 상태창:");
        sensorController.printSensorStatusTable();
        Serial.print("변경할 센서 번호들을 입력하세요 (예: 1 2 3, 취소:c): ");
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
        Serial.println("지원하지 않는 메뉴입니다. 1~6 중 선택하세요.");
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
        return;
    }

    if (!validateSensorInput())
        return;

    std::vector<int> indices = parseSensorIndices(inputBuffer);
    if (!processSensorIndices(indices))
        return;

    proceedToSensorConfirmation();
}

bool MenuController::validateSensorInput()
{
    for (char c : inputBuffer)
    {
        if (isspace(c) || (c >= '1' && c <= '8'))
            continue;
        // 1-8, 공백 이외의 문자가 있으면 오류
        Serial.println("[오류] 1~8 사이의 숫자와 공백만 입력하세요.");
        printSensorSelectionPrompt();
        return false;
    }
    return true;
}

bool MenuController::processSensorIndices(const std::vector<int>& indices)
{
    if (indices.empty())
    {
        Serial.println("[오류] 유효한 센서 번호가 없습니다.");
        printSensorSelectionPrompt();
        return false;
    }

    const auto *sortedRows = sensorController.getSortedSensorRows();
    selectedSensorIndices.clear();
    std::vector<int> invalidIndices;

    for (int idx : indices)
    {
        if (idx >= 1 && idx <= SENSOR_MAX_COUNT && sortedRows[idx - 1].connected)
        {
            selectedSensorIndices.push_back(idx);
        }
        else
        {
            invalidIndices.push_back(idx);
        }
    }

    if (!invalidIndices.empty())
    {
        Serial.print("[오류] 연결되지 않았거나 잘못된 센서 번호: ");
        for (size_t i = 0; i < invalidIndices.size(); ++i)
        {
            Serial.print(invalidIndices[i]);
            if (i < invalidIndices.size() - 1)
                Serial.print(", ");
        }
        Serial.println();
        printSensorSelectionPrompt();
        return false;
    }

    if (selectedSensorIndices.empty())
    {
        Serial.println("[오류] 선택된 센서가 없습니다.");
        printSensorSelectionPrompt();
        return false;
    }

    return true;
}

void MenuController::proceedToSensorConfirmation()
{
    // 복수 선택 모드에서만 선택된 센서 번호 안내
    if (isMultiSelectMode)
    {
        Serial.print("선택된 센서 번호: ");
        for (int idx : selectedSensorIndices)
        {
            Serial.print(idx);
            Serial.print(" ");
        }
        Serial.println();
    }

    // 첫 번째 센서부터 변경 확인 안내문 출력
    const auto *sortedRows = sensorController.getSortedSensorRows();
    selectedDisplayIdx = selectedSensorIndices[0];
    selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
    appState = AppState::SensorIdChange_ConfirmSensor;
    Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor");
    Serial.print("센서 ");
    Serial.print(selectedDisplayIdx);
    Serial.println("번을 변경할까요? (y/n, 취소:c)");
}

void MenuController::printSensorSelectionPrompt()
{
    if (isMultiSelectMode)
        Serial.print("변경할 센서 번호들을 입력하세요 (예: 1 2 3, 취소:c): ");
    else
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
}

void MenuController::handleSensorIdConfirmState()
{
    if (inputBuffer == "y" || inputBuffer == "Y")
    {
        handleConfirmYes();
    }
    else if (inputBuffer == "n" || inputBuffer == "N" || inputBuffer == "c" || inputBuffer == "C")
    {
        handleConfirmNoOrCancel();
    }
    else
    {
        handleInvalidConfirmInput();
    }
}

void MenuController::handleSensorIdInputState()
{
    if (inputBuffer == "c" || inputBuffer == "C")
    {
        handleCancelInput();
        return;
    }

    int newId = inputBuffer.toInt();
    if (newId >= 1 && newId <= SENSOR_MAX_COUNT)
    {
        processNewSensorId(newId);
    }
    else
    {
        Serial.println("[오류] ID는 1~8 사이의 숫자여야 합니다.");
        printIdInputPrompt();
    }
}

void MenuController::handleCancelInput()
{
    // 취소 시 다음 센서 확인으로 넘어감
    auto it = std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx);
    if (it != selectedSensorIndices.end())
    {
        size_t currentIndex = std::distance(selectedSensorIndices.begin(), it);
        if (currentIndex + 1 < selectedSensorIndices.size())
        {
            moveToNextSensor();
        }
        else
        {
            appState = AppState::SensorIdMenu;
            Serial.println("[DEBUG] appState -> SensorIdMenu (완료 또는 취소)");
            printSensorIdMenu();
        }
    }
}

bool MenuController::processNewSensorId(int newId)
{
    if (sensorController.isIdDuplicated(newId, selectedSensorIdx))
    {
        Serial.println("[오류] 이미 사용 중인 ID입니다.");
        printIdInputPrompt();
        return false;
    }

    sensorController.setSensorLogicalId(selectedSensorIdx, newId);
    Serial.print("센서 ");
    Serial.print(selectedDisplayIdx);
    Serial.print("의 ID를 ");
    Serial.print(newId);
    Serial.println("(으)로 변경 완료");

    completeIdChange();
    return true;
}

void MenuController::moveToNextSensor()
{
    selectedDisplayIdx = selectedSensorIndices[std::distance(selectedSensorIndices.begin(), 
        std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx)) + 1];
    const auto *sortedRows = sensorController.getSortedSensorRows();
    selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
    appState = AppState::SensorIdChange_ConfirmSensor;
    Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor (다음 센서)");
    Serial.print("센서 ");
    Serial.print(selectedDisplayIdx);
    Serial.println("번을 변경할까요? (y/n, 취소:c)");
}

void MenuController::completeIdChange()
{
    if (isMultiSelectMode)
    {
        // 복수 선택 모드: 다음 센서로 이동
        auto it = std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx);
        if (it != selectedSensorIndices.end())
        {
            size_t currentIndex = std::distance(selectedSensorIndices.begin(), it);
            if (currentIndex + 1 < selectedSensorIndices.size())
            {
                moveToNextSensor();
            }
            else
            {
                // 마지막 센서였으면 메뉴로 복귀
                appState = AppState::SensorIdMenu;
                Serial.println("[DEBUG] appState -> SensorIdMenu (모두 완료)");
                sensorController.printSensorStatusTable();
                printSensorIdMenu();
            }
        }
    }
    else
    {
        // 개별 선택 모드: 센서 선택 입력 상태로 복귀
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.println("[DEBUG] appState -> SensorIdChange_SelectSensor (개별 모드 계속)");
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력: ");
    }
}

void MenuController::printIdInputPrompt()
{
    Serial.print("센서 ");
    Serial.print(selectedDisplayIdx);
    Serial.print("의 새로운 ID(1~8, 취소:c)를 입력하세요: ");
}

void MenuController::handleConfirmYes()
{
    appState = AppState::SensorIdChange_InputId;
    Serial.println("[DEBUG] appState -> SensorIdChange_InputId");
    printIdInputPrompt();
}

void MenuController::handleConfirmNoOrCancel()
{
    // 복수 선택 모드일 때만 다음 센서로 진행, 아니면 센서 선택 입력 프롬프트로 복귀
    if (selectedSensorIndices.size() > 1)
    {
        moveToNextSensorInConfirm();
    }
    else
    {
        // 단일 선택 모드: 센서 선택 입력 프롬프트로 복귀
        appState = AppState::SensorIdChange_SelectSensor;
        Serial.print("변경할 센서 번호(1~8, 취소:c) 입력:");
    }
}

void MenuController::handleInvalidConfirmInput()
{
    Serial.println("y(예), n(아니오), c(취소) 중 하나를 입력하세요.");
    Serial.print("센서 ");
    Serial.print(selectedDisplayIdx);
    Serial.print("번을 변경할까요? (y/n, 취소:c)");
}

void MenuController::moveToNextSensorInConfirm()
{
    auto it = std::find(selectedSensorIndices.begin(), selectedSensorIndices.end(), selectedDisplayIdx);
    if (it != selectedSensorIndices.end())
    {
        size_t currentIndex = std::distance(selectedSensorIndices.begin(), it);
        if (currentIndex + 1 < selectedSensorIndices.size())
        {
            // 다음 센서로 이동
            selectedDisplayIdx = selectedSensorIndices[currentIndex + 1];
            const auto *sortedRows = sensorController.getSortedSensorRows();
            selectedSensorIdx = sortedRows[selectedDisplayIdx - 1].idx;
            appState = AppState::SensorIdChange_ConfirmSensor;
            Serial.println("[DEBUG] appState -> SensorIdChange_ConfirmSensor (다음 센서)");
            Serial.print("센서 ");
            Serial.print(selectedDisplayIdx);
            Serial.println("번을 변경할까요? (y/n, 취소:c)");
        }
        else
        {
            // 마지막 센서였으면 메뉴로 복귀
            appState = AppState::SensorIdMenu;
            Serial.println("[DEBUG] appState -> SensorIdMenu (완료 또는 취소)");
            printSensorIdMenu();
        }
    }
}
void MenuController::processInputBuffer()
{
    Serial.print("[DEBUG] appState: ");
    Serial.println((int)appState);
    Serial.print("[DEBUG] inputBuffer: ");
    Serial.println(inputBuffer);

    if (handleGlobalResetCommand())
        return;

    processStateBasedInput();
    clearInputBuffer();
}

bool MenuController::handleGlobalResetCommand()
{
    if (inputBuffer == "reset" || inputBuffer == "RESET")
    {
        Serial.println("[INFO] 강제 리셋 명령어 수신");
        resetToNormalState();
        sensorController.printSensorStatusTable();
        lastPrint = millis();
        clearInputBuffer();
        return true;
    }
    return false;
}

void MenuController::processStateBasedInput()
{
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
    default:
        // 알 수 없는 상태인 경우 강제로 Normal 상태로 리셋
        Serial.println("[경고] 알 수 없는 상태 감지, Normal 상태로 리셋합니다.");
        resetToNormalState();
        sensorController.printSensorStatusTable();
        lastPrint = millis();
        break;
    }
}

void MenuController::clearInputBuffer()
{
    inputBuffer = "";
    
    // 안전한 버퍼 클리어를 위한 제한값들
    const int MAX_CLEAR_CHARS = 64;  // 더 보수적으로 설정
    const unsigned long MAX_CLEAR_TIME_MS = 5; // 최대 클리어 시간 제한
    
    unsigned long startTime = millis();
    int clearCount = 0;
    int consecutiveFailures = 0;
    const int MAX_CONSECUTIVE_FAILURES = 3;
    
    // 입력 처리 후 Serial 버퍼 완전 비우기 (테스트 자동화 환경 대응)
    while (clearCount < MAX_CLEAR_CHARS && 
           (millis() - startTime) < MAX_CLEAR_TIME_MS &&
           consecutiveFailures < MAX_CONSECUTIVE_FAILURES)
    {
        if (!Serial.available()) {
            break; // 더 이상 읽을 데이터 없음
        }
        
        int readResult = Serial.read();
        if (readResult == -1) {
            consecutiveFailures++;
            continue; // 읽기 실패 시 재시도
        }
        
        consecutiveFailures = 0; // 성공적인 읽기 후 실패 카운터 리셋
        clearCount++;
    }
}