# 1. 声明cmake所需版本
cmake_minimum_required(VERSION 3.1.3)

# 2. 设置工程项目名称（内部会生成一系列的内置变量）
project(main VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

# 3. 添加生成目标（可执行程序）
# file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
set(src_files ./test.cc)
add_executable(${PROJECT_NAME} ${src_files})

# 4. 依赖检查（依赖库的查找以及链接选项的添加）
find_package(Bili_scaffold REQUIRED)

# 5. 设置链接库
target_link_libraries(${PROJECT_NAME} PRIVATE Bili_scaffold::bite_scaffold)
