# 定义变量，保存所有需要查找的库的名称
set(libs 
    amqpcpp 
    ev
    brpc 
    leveldb 
    protobuf 
    dl 
    ssl 
    crypto 
    gflags
    pthread
    cpr 
    jsoncpp 
    elasticlient
    cpprest 
    etcd-cpp-api
    fdfsclient 
    fastcommon
    avcodec 
    avformat
    avutil
    gtest
    curl
    odb-mysql 
    odb 
    odb-boost
    hiredis 
    redis++
    spdlog
    fmt
    boost_system
)
# 编写辅助宏：查找库
macro(find_libraries lib)
        find_library(${lib}_LIBRARY ${lib}  PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
        if(${lib}_LIBRARY)
            set(${lib}_FOUND TRUE)
            if(NOT TARGET Bili_scaffold::${lib})
                add_library(Bili_scaffold::${lib} INTERFACE IMPORTED)
                set_target_properties(Bili_scaffold::${lib} PROPERTIES
                    INTERFACE_LINK_LIBRARIES "${${lib}_LIBRARY}"
                )
                message(STATUS "找到依赖库: " ${lib} " - " ${${lib}_LIBRARY})
            endif()
        endif()
endmacro()

function(find_my_depends)
    foreach(lib ${libs})
        find_libraries(${lib})
        if(NOT ${lib}_FOUND)
            message(FATAL_ERROR "丢失关键依赖库: ${lib}")
        endif()
    endforeach()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})
endfunction()