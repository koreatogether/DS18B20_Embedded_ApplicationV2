// Minimal header-only test template for PlatformIO + Unity
// Place in test/test_header_only_minimal.cpp
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_example_pass(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_example_pass);
    return UNITY_END();
}
