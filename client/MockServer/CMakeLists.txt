cmake_minimum_required(VERSION 3.19)
project(MockServer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets HttpServer)

qt_standard_project_setup()

qt_add_executable(MockServer
    WIN32 MACOSX_BUNDLE
    main.cpp
    server.cpp
    server.h
    util.h
)

target_link_libraries(MockServer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::HttpServer
)

include(GNUInstallDirs)

install(TARGETS MockServer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MockServer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
