#include <gtest/gtest.h>

//TEST(test_my_class,googleasiotest){
//    asiotest asiotest_;
//    asiotest_.get_data();
//}
// Demonstrate some basic assertions.
TEST(test_my_class, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 7, 42);
}
