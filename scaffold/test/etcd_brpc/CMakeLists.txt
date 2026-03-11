# 1. 声明cmake所需版本
cmake_minimum_required(VERSION 3.1.3)

# 2. 设置工程项目名称（内部会生成一系列的内置变量）
project(rpc_server VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

# 3. 执行外部指令，进行proto编译
file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
execute_process(
    COMMAND protoc --experimental_allow_proto3_optional -I ${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${proto_files}
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE proto_srcs ${CMAKE_CURRENT_BINARY_DIR}/*.cc)

# 3. 添加生成目标（可执行程序）
set(server_target "rpc_server")
add_executable(${server_target} ${CMAKE_CURRENT_SOURCE_DIR}/rpc_server.cc ${proto_srcs})

set(client_target "rpc_client")
add_executable(${client_target} ${CMAKE_CURRENT_SOURCE_DIR}/rpc_client.cc ${proto_srcs})

# 4. 依赖检查（依赖库的查找以及链接选项的添加）
find_package(Bili_scaffold REQUIRED)

# 5. 设置链接库
target_link_libraries(${server_target} PRIVATE Bili_scaffold::Bili_scaffold)
target_link_libraries(${client_target} PRIVATE Bili_scaffold::Bili_scaffold)
