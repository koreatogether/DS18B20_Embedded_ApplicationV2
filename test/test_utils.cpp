#include "test_utils.h"
#include <cstdio>

std::vector<MockTemperatureSensor::Entry> getMockData()
{
    return {
        {0x01234567890, 25.5f},
        {0x01234567891, 29.1f},
        {0x01234567892, 31.2f},
        {0x01234567893, 19.5f},
        {0x01234567894, 22.0f},
        {0x01234567895, 27.8f},
        {0x01234567896, 24.3f},
        {0x01234567897, 26.7f},
    };
}

std::string makeTableLine(int idx, const MockTemperatureSensor::Entry &entry)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "%2d   | %2d  | 0x%010llX | %7.1f°C | %7.1f°C | %-6s   | %7.1f°C | %-6s   | %-6s",
             idx + 1, idx + 1, entry.address, entry.temperature, 30.0f, "정상", 20.0f, "정상", "정상");
    return std::string(buf);
}

#include <unity.h>
#include "mocks/HeaderOnlyTemperatureSensorManager.h"

void test_sensor_table_normal(void)
{
    std::vector<MockTemperatureSensor::Entry> data = {
        {0x12345678, 25.5f}, {0x87654321, 29.9f}};
    MockTemperatureSensor mockSensor(data);
    HeaderOnlyTemperatureSensorManager manager(&mockSensor);
    manager.update();
    TEST_ASSERT_EQUAL(manager.getSensorCount(), 2);
    auto s0 = manager.getSensorStatus(0);
    auto s1 = manager.getSensorStatus(1);
    TEST_ASSERT_NOT_NULL(s0);
    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_EQUAL(s0->id, 1);
    TEST_ASSERT_EQUAL(s1->id, 2);
    TEST_ASSERT_EQUAL_FLOAT(s0->temperature, 25.5f);
    TEST_ASSERT_EQUAL_FLOAT(s1->temperature, 29.9f);
    TEST_ASSERT_EQUAL_STRING(s0->upperState.c_str(), "정상");
    TEST_ASSERT_EQUAL_STRING(s1->upperState.c_str(), "정상");
}

void test_sensor_table_na(void)
{
    std::vector<MockTemperatureSensor::Entry> data = {};
    MockTemperatureSensor mockSensor(data);
    HeaderOnlyTemperatureSensorManager manager(&mockSensor);
    manager.update();
    TEST_ASSERT_EQUAL(manager.getSensorCount(), 0);
    auto s0 = manager.getSensorStatus(0);
    TEST_ASSERT_NULL(s0);
}

void test_sensor_table_periodic_update(void)
{
    std::vector<MockTemperatureSensor::Entry> data = {
        {0x12345678, 25.5f}};
    MockTemperatureSensor mockSensor(data);
    HeaderOnlyTemperatureSensorManager manager(&mockSensor);
    for (int i = 0; i < 3; ++i)
    {
        manager.update();
        auto s0 = manager.getSensorStatus(0);
        TEST_ASSERT_NOT_NULL(s0);
        TEST_ASSERT_EQUAL_FLOAT(s0->temperature, 25.5f);
    }
    // 실제 타이머/지연은 임베디드 환경에서 별도 검증 필요
}
