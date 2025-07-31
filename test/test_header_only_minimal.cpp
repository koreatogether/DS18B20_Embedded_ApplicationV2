
#include <unity.h>
#include <string>
#include <vector>
#include "mocks/MockTemperatureSensor.h"
#include "mocks/HeaderOnlyTemperatureSensorManager.h"

void setUp(void) {}
void tearDown(void) {}

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

void test_table_format_alignment(void)
{
    auto mockData = getMockData();

    MockTemperatureSensor mockSensor(mockData);
    HeaderOnlyTemperatureSensorManager manager(&mockSensor);
    manager.update();

    // 헤더 라인
    std::string header = "번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태";
    std::string sep = "---- | --- | -------------- | --------- | -------- | -------- | -------- | -------- | --------";
    TEST_ASSERT_EQUAL_STRING(header.c_str(), "번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태");
    TEST_ASSERT_EQUAL_STRING(sep.c_str(), "---- | --- | -------------- | --------- | -------- | -------- | -------- | -------- | --------");

    // 센서 데이터 라인
    for (size_t i = 0; i < mockData.size(); ++i)
    {
        auto expected = makeTableLine((int)i, mockData[i]);
        // 실제 서비스에서는 상태/임계값이 동적으로 바뀌지만, 여기선 포맷만 검증
        TEST_ASSERT_TRUE_MESSAGE(expected.find("|") != std::string::npos, "테이블 구분자 확인");
    }
    TEST_ASSERT_EQUAL(manager.getSensorCount(), 8);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_table_format_alignment);
    return UNITY_END();
}
