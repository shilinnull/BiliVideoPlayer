#include<iostream>
#include<gtest/gtest.h>

int abs(int x)
{
    return x > 0 ? x : -x;
}

TEST(abs_test, test1)
{
    ASSERT_TRUE(abs(1) == 1) << "abs(1) = 1";
    ASSERT_TRUE(abs(-1) == 1);
    ASSERT_FALSE(abs(-2) == -2);
    ASSERT_EQ(abs(1), abs(-1));
    ASSERT_NE(abs(-1), 0);
    ASSERT_LT(abs(-1), 2);
    ASSERT_GT(abs(-1), 0);
    ASSERT_LE(abs(-1), 2);
    ASSERT_GE(abs(-1), 0);
}

int main(int argc, char *argv[])
{
    //将命令行参数传递给gtest
    testing::InitGoogleTest(&argc, argv);
    // 运行所有测试案例
    return RUN_ALL_TESTS();
}