#include <iostream>
#include <gtest/gtest.h>
#include <unordered_map>

class HashTestEnv2 : public testing::Test {
    public:
        static void SetUpTestCase() {
            std::cout << "环境2第一个TEST之前被调用,进行总体环境配置\n";
        }
        static void TearDownTestCase() {
            std::cout << "环境2最后一个TEST之后被调用,进行总体环境清理\n";
        }
        virtual void SetUp() override {
            std::cout << "环境2测试前:提前准备数据!!\n";
            dict.insert(std::make_pair("bye", "再见"));
            dict.insert(std::make_pair("see you", "再见"));
        }
        virtual void TearDown() override {
            std::cout << "环境2测试结束后:清理数据!!\n";
            dict.clear();
        }
    public:
        std::unordered_map<std::string, std::string> dict;
};

TEST_F(HashTestEnv2, insert_test) {
    std::cout << "环境2,中间测试\n";
    dict.insert(std::make_pair("hello", "你好"));
    ASSERT_EQ(dict.size(), 3);
}

TEST_F(HashTestEnv2, size_test) {
    std::cout << "环境2,中间size测试\n";
    auto it = dict.find("hello");
    ASSERT_EQ(it, dict.end());
    ASSERT_EQ(dict.size(), 2);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}