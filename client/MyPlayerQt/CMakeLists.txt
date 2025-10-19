cmake_minimum_required(VERSION 3.19)
project(MyPlayerQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
# Qt MultimediaWidgets provides QVideoWidget and related UI classes
find_package(Qt6 REQUIRED COMPONENTS Multimedia MultimediaWidgets)

qt_standard_project_setup()

qt_add_executable(MyPlayerQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    myplayerqt.cpp
    myplayerqt.h
)

target_link_libraries(MyPlayerQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)

include(GNUInstallDirs)

install(TARGETS MyPlayerQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MyPlayerQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
