# 1. 声明cmake所需版本
cmake_minimum_required(VERSION 3.1.3)
# 2. 设置工程项目名称（内部会生成一系列的内置变量）
project(scaffold VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
# 3. 添加生成目标（可执行程序）
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/scaffold.cc ${CMAKE_CURRENT_BINARY_DIR}/cal.pb.cc)
# 4. 执行外部命令，生成临时代码文件: cal.pb.h cal.pb.cc
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cal.pb.cc
    COMMAND protoc --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I ${CMAKE_CURRENT_SOURCE_DIR} cal.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cal.proto
)
# 5. 头文件路径设置
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# 6. 依赖检查（依赖库的查找以及链接选项的添加）
find_package(Bili_scaffold REQUIRED)
# 7. 设置链接库
target_link_libraries(${PROJECT_NAME} PRIVATE Bili_scaffold::Bili_scaffold)
# 8. 安装可执行程序
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)