# 1. 设置cmake所需版本
cmake_minimum_required(VERSION 3.1.3)
# 2. 设置工程项目名称（内部会生成一系列的内置变量）
project(Bili_scaffold VERSION 1.0)
# 3. 添加生成库目标（说明通过哪些文件生成哪个库）
file(GLOB_RECURSE BASE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc) # CMAKE_CURRENT_SOURCE_DIR cmakelists.txt文件在哪里，就指向哪个路径
add_library(${PROJECT_NAME} STATIC ${BASE_FILES})
# 4. 设置编译特性
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
# 5. 设置生成库属性：默认生成位置，版本信息
set_target_properties(${PROJECT_NAME} PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)  # CMAKE_CURRENT_BINARY_DIR 在哪里执行cmake，就指向哪个路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}  # 完整版本号（1.0）
    SOVERSION ${PROJECT_VERSION_MAJOR}  # 主版本号（1）
)
# 6. 依赖检查
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # 声明依赖检测子模块的所在路径
include(Finddepends)
#   1. 编写依赖检查子模块（查找系统中的指定库，并将库添加到链接选项中）
find_my_depends()
# 7. 设置安装头文件到指定位置
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
)
# 8. 设置安装库文件到指定位置
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets # 声明目标名称
    ARCHIVE DESTINATION lib  #静态库安装路径
    INCLUDES DESTINATION include/${PROJECT_NAME} #头文件安装路径
)
# 9. 设置安装子模块到指定位置
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/
    DESTINATION lib/cmake/${PROJECT_NAME}/modules
    FILES_MATCHING PATTERN "Find*.cmake"
)
# 10. 设置导出目标信息
install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
)
# 11. 处理 Config.cmake.in 模板
#   1. 需要提前编写好模板内容
include(CMakePackageConfigHelpers)
# 处理 Config.cmake.in 模板，生成最终的配置文件，解析 @PACKAGE_INIT@ 变量
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)
# 12. 设置版本配置文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
# 13. 安装配置文件
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)