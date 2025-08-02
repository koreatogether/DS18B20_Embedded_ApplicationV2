#pragma once
#include <unity.h>
#include <string>
#include <vector>
#include "../mocks/MockMenuController.h"
#include "../mocks/MockSensorController.h"

// 03 시나리오에서 발견된 구체적인 문제들을 테스트하는 함수들

// 03 시나리오에서 발견된 복합 센서 ID 변경 플로우 테스트
inline void test_scenario_03_complex_flow(void)
{
    MenuController menuController;
    SensorController sensorController;

    // 1. 초기 상태는 Normal
    TEST_ASSERT_EQUAL_INT((int)AppState::Normal, (int)menuController.getAppState());

    // 2. menu 명령으로 메뉴 진입
    menuController.setAppState(AppState::Menu);
    menuController.printMenu();
    std::string output = menuController.getMockOutput();
    TEST_ASSERT_TRUE(output.find("센서 제어 메뉴") != std::string::npos);

    // 3. "1" 입력으로 센서 ID 조정 메뉴로 이동
    menuController.clearMockOutput();
    menuController.simulateMenuInput("1");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdMenu, (int)menuController.getAppState());

    // 4. 센서 ID 메뉴 출력 확인
    menuController.printSensorIdMenu();
    output = menuController.getMockOutput();
    TEST_ASSERT_TRUE(output.find("센서 ID 조정 메뉴") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("2. 복수의 센서 ID 변경") != std::string::npos);

    // 5. "2" 입력으로 복수 센서 ID 변경 모드로 진입
    menuController.clearMockOutput();
    menuController.simulateMenuInput("2");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_SelectSensor, (int)menuController.getAppState());
    TEST_ASSERT_TRUE(menuController.getIsMultiSelectMode());

    // 6. 복수 센서 선택 "1 2 3" (03 시나리오에서 사용된 패턴)
    menuController.simulateSensorSelection("1 2 3");
    auto indices = menuController.getSelectedSensorIndices();
    TEST_ASSERT_EQUAL_INT(3, indices.size());
    TEST_ASSERT_EQUAL_INT(1, indices[0]);
    TEST_ASSERT_EQUAL_INT(2, indices[1]);
    TEST_ASSERT_EQUAL_INT(3, indices[2]);
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_ConfirmSensor, (int)menuController.getAppState());
}

// 03 시나리오에서 발견된 센서 상태 테이블 출력 형식 검증
inline void test_scenario_03_table_format(void)
{
    SensorController controller;

    controller.printSensorStatusTable();
    std::string output = controller.getMockOutput();

    // 테이블 헤더 형식 검증 (03 시나리오에서 실제 기대값과 매칭되는지 확인)
    TEST_ASSERT_TRUE(output.find("| 번호 | ID  | 주소           | 온도      | 상한     | 상한상태 | 하한     | 하한상태 | 상태     |") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("|------|-----|----------------|-----------|----------|----------|----------|----------|----------|") != std::string::npos);

    // 연결된 센서 행 형식 검증
    TEST_ASSERT_TRUE(output.find("°C") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("0x") != std::string::npos); // 주소 형식

    // 미연결 센서 행 형식 검증 (N/A 값들)
    size_t naCount = 0;
    size_t pos = 0;
    while ((pos = output.find("N/A", pos)) != std::string::npos)
    {
        naCount++;
        pos++;
    }
    TEST_ASSERT_TRUE(naCount > 0); // 미연결 센서들에 대해 N/A가 나타나야 함
}

// 03 시나리오에서 발견된 공백/인코딩 문제를 시뮬레이션
inline void test_scenario_03_whitespace_robustness(void)
{
    MenuController controller;

    // 다양한 입력 형식에 대한 robustness 테스트
    auto result1 = MenuController::parseSensorIndices("1 2 3");   // 공백 구분
    auto result2 = MenuController::parseSensorIndices("1,2,3");   // 콤마 구분
    auto result3 = MenuController::parseSensorIndices("1, 2, 3"); // 콤마 + 공백
    auto result4 = MenuController::parseSensorIndices("123");     // 붙여서

    // 모든 경우에서 동일한 결과 (1, 2, 3)를 얻어야 함
    TEST_ASSERT_EQUAL_INT(3, result1.size());
    TEST_ASSERT_EQUAL_INT(3, result2.size());
    TEST_ASSERT_EQUAL_INT(3, result3.size());
    TEST_ASSERT_EQUAL_INT(3, result4.size());

    for (int i = 0; i < 3; ++i)
    {
        TEST_ASSERT_EQUAL_INT(i + 1, result1[i]);
        TEST_ASSERT_EQUAL_INT(i + 1, result2[i]);
        TEST_ASSERT_EQUAL_INT(i + 1, result3[i]);
        TEST_ASSERT_EQUAL_INT(i + 1, result4[i]);
    }
}

// 03 시나리오에서 발견된 상태 전환 연쇄를 테스트
inline void test_scenario_03_state_chain(void)
{
    MenuController controller;

    // Normal -> Menu -> SensorIdMenu -> SelectSensor -> ConfirmSensor 연쇄
    TEST_ASSERT_EQUAL_INT((int)AppState::Normal, (int)controller.getAppState());

    controller.setAppState(AppState::Menu);
    controller.simulateMenuInput("1");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdMenu, (int)controller.getAppState());

    controller.simulateMenuInput("1"); // 개별 센서 선택
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_SelectSensor, (int)controller.getAppState());
    TEST_ASSERT_FALSE(controller.getIsMultiSelectMode());

    // 센서 선택
    controller.simulateSensorSelection("3");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_ConfirmSensor, (int)controller.getAppState());
    TEST_ASSERT_EQUAL_INT(3, controller.getSelectedDisplayIdx());

    // 뒤로 가기 테스트 ("6" -> Normal로 직접 복귀)
    controller.setAppState(AppState::SensorIdMenu);
    controller.simulateMenuInput("6");
    TEST_ASSERT_EQUAL_INT((int)AppState::Normal, (int)controller.getAppState());
}

// 03 시나리오에서 제거된 불필요한 step 검증
inline void test_scenario_03_unnecessary_steps_avoided(void)
{
    MenuController controller;
    SensorController sensorController;

    // 상태창에서 아무 출력도 없는 명령들은 처리하지 않아야 함
    controller.setAppState(AppState::Normal);

    // Normal 상태에서 숫자 입력은 아무 반응이 없어야 함
    std::string beforeOutput = controller.getMockOutput();
    controller.handleSerialInput(); // 빈 입력 처리
    std::string afterOutput = controller.getMockOutput();

    // 출력이 변경되지 않아야 함 (불필요한 step으로 인한 혼란 방지)
    TEST_ASSERT_EQUAL_STRING(beforeOutput.c_str(), afterOutput.c_str());
}

// SensorController의 자동 주소순 할당 기능 테스트 (03 시나리오에서 사용됨)
inline void test_scenario_03_auto_assign_by_address(void)
{
    SensorController controller;

    // 자동 주소순 할당 전 상태 확인
    const auto *beforeRows = controller.getSortedSensorRows();
    std::vector<int> beforeIds;
    for (int i = 0; i < 4; ++i)
    {
        if (beforeRows[i].connected)
        {
            beforeIds.push_back(beforeRows[i].logicalId);
        }
    }

    // 자동 할당 실행
    controller.clearMockOutput();
    controller.assignIDsByAddress();
    std::string output = controller.getMockOutput();

    // 성공 메시지 확인
    TEST_ASSERT_TRUE(output.find("[자동] 주소순 ID 할당 완료") != std::string::npos);

    // 할당 후 ID가 순차적으로 정렬되었는지 확인
    const auto *afterRows = controller.getSortedSensorRows();
    for (int i = 0; i < 4; ++i)
    {
        if (afterRows[i].connected)
        {
            TEST_ASSERT_EQUAL_INT(i + 1, afterRows[i].logicalId);
        }
    }
}
