cmake_minimum_required(VERSION 3.19)
project(BiliVideoPlayer LANGUAGES CXX)

set(APP_NAME BiliVideoPlayer)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --use-temp-file")

set(APP_WIN_RC "")
if(WIN32)
    set(APP_WIN_RC ${CMAKE_CURRENT_SOURCE_DIR}/biliPlayer.rc)
endif()

set(APP_SOURCES
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
    modifymyselfdialog.h modifymyselfdialog.cpp modifymyselfdialog.ui
    newpassworddialog.h newpassworddialog.cpp newpassworddialog.ui
    uploadvideopage.h uploadvideopage.cpp uploadvideopage.ui
    adminwidget.h adminwidget.cpp adminwidget.ui
    checktable.h checktable.cpp checktable.ui
    roletable.h roletable.cpp roletable.ui
    checktableitem.h checktableitem.cpp checktableitem.ui
    roletableitem.h roletableitem.cpp roletableitem.ui
    edituserdialog.h edituserdialog.cpp edituserdialog.ui
    pagebutton.h pagebutton.cpp
    paginator.h paginator.cpp
    login.h login.cpp login.ui
    toast.h toast.cpp
    player/player.cpp player/player.h
    player/audiodecoder.cpp player/audiodecoder.h
    player/avpacketqueue.cpp player/avpacketqueue.h
    bulletscreenitem.h bulletscreenitem.cpp
    barrageedit.h barrageedit.cpp
    model/datacenter.h model/datacenter.cpp
    model/data.h model/data.cpp
    netclient/netclient.h netclient/netclient.cpp
    confirmdialog.h confirmdialog.cpp
    ${APP_WIN_RC}
)

set(APP_RESOURCES
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
    images/biliPlayer.ico
)

qt_standard_project_setup()

qt_add_executable(${APP_NAME}
    WIN32 MACOSX_BUNDLE
    ${APP_SOURCES}
)

target_include_directories(${APP_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
    ${PROJECT_SOURCE_DIR}/sdl/include
)

qt_add_resources(
    ${APP_NAME}
    PREFIX "/"
    FILES
    ${APP_RESOURCES}
)

target_link_libraries(${APP_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        avcodec
        avformat
        avfilter
        avutil
        swresample
        swscale
        SDL2
)

target_link_directories(${APP_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/ffmpeg/lib
        ${PROJECT_SOURCE_DIR}/sdl/lib
)

set(FFMPEG_DLL_DIR "${PROJECT_SOURCE_DIR}/dll")
set(FFMPEG_DLLS
    "${FFMPEG_DLL_DIR}/avcodec-58.dll"
    "${FFMPEG_DLL_DIR}/avdevice-58.dll"
    "${FFMPEG_DLL_DIR}/avfilter-7.dll"
    "${FFMPEG_DLL_DIR}/avformat-58.dll"
    "${FFMPEG_DLL_DIR}/avutil-56.dll"
    "${FFMPEG_DLL_DIR}/postproc-55.dll"
    "${FFMPEG_DLL_DIR}/swresample-3.dll"
    "${FFMPEG_DLL_DIR}/swscale-5.dll"
)
set(SDL_DLL "${FFMPEG_DLL_DIR}/SDL2.dll")

if(WIN32)
    foreach(DLL_FILE IN LISTS FFMPEG_DLLS)
        if(EXISTS "${DLL_FILE}")
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_FILE}"
                    "$<TARGET_FILE_DIR:${APP_NAME}>"
            )
        endif()
    endforeach()
    if(EXISTS "${SDL_DLL}")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SDL_DLL}"
                "$<TARGET_FILE_DIR:${APP_NAME}>"
        )
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS ${APP_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(WIN32)
    foreach(DLL_FILE IN LISTS FFMPEG_DLLS)
        if(EXISTS "${DLL_FILE}")
            install(FILES "${DLL_FILE}" DESTINATION ${CMAKE_INSTALL_BINDIR})
        endif()
    endforeach()
    if(EXISTS "${SDL_DLL}")
        install(FILES "${SDL_DLL}" DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()

qt_generate_deploy_app_script(
    TARGET ${APP_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
