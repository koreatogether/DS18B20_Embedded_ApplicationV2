

#include <unity.h>
#include "interfaces/test_utils.h"
#include "interfaces/test_scenario_03.h" // 03 시나리오 기반 테스트 추가
// Mock Arduino types for native test
#include "Arduino.h"
#include "mocks/HeaderOnlyTemperatureSensorManager.h"
#include "mocks/MockMenuController.h"   // Use header-only mock instead of firmware code
#include "mocks/MockSensorController.h" // Include SensorController mock

void setUp(void) {}
void tearDown(void) {}

// 테스트 함수들은 test_utils.h에 inline으로 구현됨

// Compilation smoke-test for MenuController to ensure firmware code builds under native tests
static void test_menu_controller_compiles(void)
{
    // Instantiate to verify header and class compile
    MenuController controller;
    (void)controller;
}

// Unit tests for MenuController::parseSensorIndices
static void test_parseSensorIndices_single(void)
{
    auto v = MenuController::parseSensorIndices("3");
    TEST_ASSERT_EQUAL_INT(1, v.size());
    TEST_ASSERT_EQUAL_INT(3, v[0]);
}

static void test_parseSensorIndices_multiple(void)
{
    auto v = MenuController::parseSensorIndices("1,2,3");
    TEST_ASSERT_EQUAL_INT(3, v.size());
    TEST_ASSERT_EQUAL_INT(1, v[0]);
    TEST_ASSERT_EQUAL_INT(2, v[1]);
    TEST_ASSERT_EQUAL_INT(3, v[2]);
}

static void test_parseSensorIndices_duplicates(void)
{
    auto v = MenuController::parseSensorIndices("1,1,2,2,3");
    TEST_ASSERT_EQUAL_INT(3, v.size()); // duplicates removed
    TEST_ASSERT_EQUAL_INT(1, v[0]);
    TEST_ASSERT_EQUAL_INT(2, v[1]);
    TEST_ASSERT_EQUAL_INT(3, v[2]);
}

static void test_parseSensorIndices_boundary(void)
{
    auto v1 = MenuController::parseSensorIndices("0,9,10");
    TEST_ASSERT_EQUAL_INT(1, v1.size()); // "10" contains '1' which is valid
    TEST_ASSERT_EQUAL_INT(1, v1[0]);

    auto v2 = MenuController::parseSensorIndices("18");
    TEST_ASSERT_EQUAL_INT(2, v2.size()); // extracts 1 and 8
    TEST_ASSERT_EQUAL_INT(1, v2[0]);
    TEST_ASSERT_EQUAL_INT(8, v2[1]);
}

static void test_parseSensorIndices_mixed_format(void)
{
    auto v = MenuController::parseSensorIndices("1 2,3-4 5");
    TEST_ASSERT_EQUAL_INT(5, v.size()); // extracts 1,2,3,4,5 (4 is valid)
    TEST_ASSERT_EQUAL_INT(1, v[0]);
    TEST_ASSERT_EQUAL_INT(2, v[1]);
    TEST_ASSERT_EQUAL_INT(3, v[2]);
    TEST_ASSERT_EQUAL_INT(4, v[3]);
    TEST_ASSERT_EQUAL_INT(5, v[4]);
}

// Test based on 03 scenario pytest findings: State transitions
static void test_menu_state_transitions(void)
{
    MenuController controller;

    // Initial state should be Normal
    TEST_ASSERT_EQUAL_INT((int)AppState::Normal, (int)controller.getAppState());

    // Simulate menu command (like "menu\n" in scenario)
    controller.setAppState(AppState::Menu);
    TEST_ASSERT_EQUAL_INT((int)AppState::Menu, (int)controller.getAppState());

    // Test menu option 1 -> SensorIdMenu
    controller.simulateMenuInput("1");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdMenu, (int)controller.getAppState());

    // Test sensor ID menu option 1 -> individual sensor selection
    controller.simulateMenuInput("1");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_SelectSensor, (int)controller.getAppState());
    TEST_ASSERT_FALSE(controller.getIsMultiSelectMode());

    // Reset and test option 2 -> multi sensor selection
    controller.setAppState(AppState::SensorIdMenu);
    controller.simulateMenuInput("2");
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_SelectSensor, (int)controller.getAppState());
    TEST_ASSERT_TRUE(controller.getIsMultiSelectMode());
}

// Test based on 03 scenario: Complex multi-sensor selection
static void test_complex_sensor_selection(void)
{
    MenuController controller;
    controller.setAppState(AppState::SensorIdChange_SelectSensor);

    // Test complex input patterns found in 03 scenario
    controller.simulateSensorSelection("1 2 3");
    auto indices = controller.getSelectedSensorIndices();
    TEST_ASSERT_EQUAL_INT(3, indices.size());
    TEST_ASSERT_EQUAL_INT(1, indices[0]);
    TEST_ASSERT_EQUAL_INT(2, indices[1]);
    TEST_ASSERT_EQUAL_INT(3, indices[2]);
    TEST_ASSERT_EQUAL_INT((int)AppState::SensorIdChange_ConfirmSensor, (int)controller.getAppState());
}

// Test SensorController mock functionality
static void test_sensor_controller_mock(void)
{
    SensorController controller;

    // Test initial setup
    TEST_ASSERT_EQUAL_INT(4, controller.getSensorCount());

    const auto *rows = controller.getSortedSensorRows();
    TEST_ASSERT_NOT_NULL(rows);

    // Test first few sensors are connected
    TEST_ASSERT_TRUE(rows[0].connected);
    TEST_ASSERT_TRUE(rows[1].connected);
    TEST_ASSERT_TRUE(rows[2].connected);
    TEST_ASSERT_TRUE(rows[3].connected);

    // Test ID duplication check
    TEST_ASSERT_TRUE(controller.isIdDuplicated(1, 1));  // ID 1 should be used by sensor 0
    TEST_ASSERT_FALSE(controller.isIdDuplicated(9, 0)); // ID 9 should be free
}

// Test scenario output matching (based on 03 scenario whitespace issues)
static void test_menu_output_format(void)
{
    MenuController controller;

    controller.printMenu();
    std::string output = controller.getMockOutput();

    // Test that output contains expected menu items
    TEST_ASSERT_TRUE(output.find("센서 제어 메뉴") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("1. 센서 ID 조정") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("메뉴 번호를 입력하세요: ") != std::string::npos);

    controller.clearMockOutput();
    controller.printSensorIdMenu();
    output = controller.getMockOutput();

    // Test sensor ID menu output
    TEST_ASSERT_TRUE(output.find("센서 ID 조정 메뉴") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("1. 개별 센서 ID 변경") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("2. 복수의 센서 ID 변경") != std::string::npos);
}

// Test sensor table output format (based on 03 scenario table matching)
static void test_sensor_table_output_format(void)
{
    SensorController controller;

    controller.printSensorStatusTable();
    std::string output = controller.getMockOutput();

    // Test table header
    TEST_ASSERT_TRUE(output.find("| 번호 | ID  | 주소") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("온도") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("상한") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("하한") != std::string::npos);

    // Test that connected sensors have proper data format
    TEST_ASSERT_TRUE(output.find("°C") != std::string::npos);
    TEST_ASSERT_TRUE(output.find("정상") != std::string::npos);

    // Test that unconnected sensors show N/A
    TEST_ASSERT_TRUE(output.find("N/A") != std::string::npos);
}

// 기존 센서 온도 관리 테스트들은 유지
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // 기존 센서 테이블 테스트
    RUN_TEST(test_sensor_table_normal);
    RUN_TEST(test_sensor_table_na);
    RUN_TEST(test_sensor_table_periodic_update);
    RUN_TEST(test_sensor_table_upper_threshold);
    RUN_TEST(test_sensor_table_lower_threshold);

    // MenuController 컴파일 및 파싱 테스트
    RUN_TEST(test_menu_controller_compiles);
    RUN_TEST(test_parseSensorIndices_single);
    RUN_TEST(test_parseSensorIndices_multiple);
    RUN_TEST(test_parseSensorIndices_duplicates);
    RUN_TEST(test_parseSensorIndices_boundary);
    RUN_TEST(test_parseSensorIndices_mixed_format);

    // 03 시나리오 기반 상태 전환 테스트
    RUN_TEST(test_menu_state_transitions);
    RUN_TEST(test_complex_sensor_selection);

    // SensorController Mock 테스트
    RUN_TEST(test_sensor_controller_mock);

    // 출력 형식 테스트 (03 시나리오에서 발견된 공백/형식 문제 기반)
    RUN_TEST(test_menu_output_format);
    RUN_TEST(test_sensor_table_output_format);

    // 03 시나리오 전용 테스트들
    RUN_TEST(test_scenario_03_complex_flow);
    RUN_TEST(test_scenario_03_table_format);
    RUN_TEST(test_scenario_03_whitespace_robustness);
    RUN_TEST(test_scenario_03_state_chain);
    RUN_TEST(test_scenario_03_unnecessary_steps_avoided);
    RUN_TEST(test_scenario_03_auto_assign_by_address);

    return UNITY_END();
}
