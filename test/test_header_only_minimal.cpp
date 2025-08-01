

#include <unity.h>
#include "interfaces/test_utils.h"
#include "mocks/HeaderOnlyTemperatureSensorManager.h"

void setUp(void) {}
void tearDown(void) {}

// 테스트 함수들은 test_utils.h에 inline으로 구현됨

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_sensor_table_normal);
    RUN_TEST(test_sensor_table_na);
    RUN_TEST(test_sensor_table_periodic_update);
    RUN_TEST(test_sensor_table_upper_threshold);
    RUN_TEST(test_sensor_table_lower_threshold);
    return UNITY_END();
}
