//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char **argv) {

    std::cout << "main test" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}
