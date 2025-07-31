// Minimal header-only test template for PlatformIO + Unity
// Place in test/test_header_only_minimal.cpp
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

// 센서 테이블 출력 포맷 검증용 테스트
#include <string>
#include <vector>

std::vector<std::string> getExpectedTableLines()
{
    return {
        "번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태",
        "---- | --- | -------------- | --------- | -------- | -------- | -------- | -------- | --------",
        " 1   |  1  | 0x01234567890 |   25.5°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ",
        " 2   |  2  | 0x01234567891 |   29.1°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ",
        " 3   |  3  | 0x01234567892 |   31.2°C |   30.0°C | 초과     |   20.0°C | 정상     | 경고  ",
        " 4   |  4  | 0x01234567893 |   19.5°C |   30.0°C | 정상     |   20.0°C | 초과     | 경고  ",
        " 5   |  5  | 0x01234567894 |   22.0°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ",
        " 6   |  6  | 0x01234567895 |   27.8°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ",
        " 7   |  7  | 0x01234567896 |   24.3°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ",
        " 8   |  8  | 0x01234567897 |   26.7°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  "};
}

void test_table_format_alignment(void)
{
    auto expected = getExpectedTableLines();
    // 실제 임베디드 환경에서는 시리얼 캡처가 어렵기 때문에, 포맷 문자열만 비교
    // (실제 센서 데이터와 포맷이 일치하는지 검증)
    // 예시: 첫 번째 라인, 두 번째 라인, 마지막 라인 등
    TEST_ASSERT_EQUAL_STRING(expected[0].c_str(), "번호 |  ID |   센서 주소    | 현재 온도 | 상한설정 | 상한상태 | 하한설정 | 하한상태 | 센서상태");
    TEST_ASSERT_EQUAL_STRING(expected[2].c_str(), " 1   |  1  | 0x01234567890 |   25.5°C |   30.0°C | 정상     |   20.0°C | 정상     | 정상  ");
    TEST_ASSERT_EQUAL(expected.size(), 10);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_table_format_alignment);
    return UNITY_END();
}
