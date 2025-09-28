cmake_minimum_required(VERSION 3.19)
project(BiliVideoPlayerSystem LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(BiliVideoPlayerSystem
    WIN32 MACOSX_BUNDLE
    main.cpp
    bilivideoplayer.cpp bilivideoplayer.h bilivideoplayer.ui
    startuppage.h startuppage.cpp
    pageswitchbutton.h pageswitchbutton.cpp
    util.h
    homepagewidget.h homepagewidget.cpp homepagewidget.ui
    searchlineedit.h searchlineedit.cpp
    videobox.h videobox.cpp videobox.ui
    playerpage.h playerpage.cpp playerpage.ui
    playslider.h playslider.cpp playslider.ui
    volume.h volume.cpp volume.ui
    playspeed.h playspeed.cpp playspeed.ui
    myselfwidget.h myselfwidget.cpp myselfwidget.ui
    avatarbutton.h avatarbutton.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

qt_add_resources(
    BiliVideoPlayerSystem
    PREFIX "/"
    FILES
    images/biliPlayer.png
    images/admin/add.png
    images/admin/arrow-left.png
    images/admin/arrow-right.png
    images/admin/checked.png
    images/admin/defaultVideoCover.png
    images/admin/triangle.png
    images/admin/triangle2.png
    images/admin/unchecked.png
    images/homePage/admin.png
    images/homePage/adminxuan.png
    images/homePage/bilishipin.png
    images/homePage/bofang.png
    images/homePage/dainzan.png
    images/homePage/logo.png
    images/homePage/quxiao.png
    images/homePage/shouye.png
    images/homePage/shouyexuan.png
    images/homePage/shuaxin.png
    images/homePage/sousuo.png
    images/homePage/suoxiao.png
    images/homePage/touxiang.png
    images/homePage/videoImage.png
    images/homePage/wode.png
    images/homePage/wodexuan.png
    images/homePage/zhiding.png
    images/login/bilishipin.png
    images/login/quxiao.png
    images/login/suoxiao.png
    images/login/xiala.png
    images/myself/bianji.png
    images/myself/defaultAvatar.png
    images/myself/gengduo.png
    images/myself/guanzhu.png
    images/myself/shangchuanshipin.png
    images/myself/shezhi.png
    images/myself/touxiang.png
    images/myself/xiugai.png
    images/myself/xiugaimima.png
    images/PlayPage/bofang.png
    images/PlayPage/bofangliang.png
    images/PlayPage/danmu.png
    images/PlayPage/danmuguan.png
    images/PlayPage/dianzan.png
    images/PlayPage/fanhui.png
    images/PlayPage/quxiao.png
    images/PlayPage/quxiaodianzan.png
    images/PlayPage/shnegyin.png
    images/PlayPage/suoxiao.png
    images/PlayPage/touxiang.png
    images/PlayPage/zanting.png
    images/startupPage/bilishipin.png
    images/uploadVideoPage/fengmian.png
    images/uploadVideoPage/huan.png
    images/uploadVideoPage/videoCoverBg.png
    images/uploadVideoPage/wancheng.png
    images/uploadVideoPage/wenjian.png
    images/uploadVideoPage/xiala.png
    images/uploadVideoPage/xuanze.png
)

target_link_libraries(BiliVideoPlayerSystem
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(BiliVideoPlayerSystem PRIVATE Qt6::Widgets)
target_link_libraries(BiliVideoPlayerSystem PRIVATE Qt6::Widgets)
target_link_libraries(BiliVideoPlayerSystem PRIVATE Qt6::Widgets)
target_link_libraries(BiliVideoPlayerSystem PRIVATE Qt6::Widgets)

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
