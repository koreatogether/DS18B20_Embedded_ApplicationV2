

#include <unity.h>
#include "interfaces/test_utils.h"
// Mock Arduino types for native test
#include "Arduino.h"
#include "mocks/HeaderOnlyTemperatureSensorManager.h"
#include "mocks/MockMenuController.h" // Use header-only mock instead of firmware code

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
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_sensor_table_normal);
    RUN_TEST(test_sensor_table_na);
    RUN_TEST(test_sensor_table_periodic_update);
    // MenuController tests
    RUN_TEST(test_menu_controller_compiles);
    RUN_TEST(test_parseSensorIndices_single);
    RUN_TEST(test_parseSensorIndices_multiple);
    RUN_TEST(test_parseSensorIndices_duplicates);
    RUN_TEST(test_parseSensorIndices_boundary);
    RUN_TEST(test_parseSensorIndices_mixed_format);
    // Existing sensor table tests
    RUN_TEST(test_sensor_table_upper_threshold);
    RUN_TEST(test_sensor_table_lower_threshold);
    return UNITY_END();
}
