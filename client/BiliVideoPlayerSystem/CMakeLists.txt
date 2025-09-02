cmake_minimum_required(VERSION 3.19)
project(BiliVideoPlayerSystem LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(BiliVideoPlayerSystem
    WIN32 MACOSX_BUNDLE
    main.cpp
    bilivideoplayer.cpp
    bilivideoplayer.h
    bilivideoplayer.ui
    imageRes.qrc
)

target_link_libraries(BiliVideoPlayerSystem
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS BiliVideoPlayerSystem
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET BiliVideoPlayerSystem
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
