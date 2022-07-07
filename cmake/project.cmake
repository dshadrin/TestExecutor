set(DestinationDir ${CMAKE_SOURCE_DIR}/dist)
message(STATUS "DestinationDir: ${DestinationDir}")

set(PRJ_QT "Qt6")
set(PRJ_BOOST_VERSION "1.79")

if(UNIX)
    message(STATUS "Toolset: Uinx GCC")
    add_definitions(-DBOOST_NO_CXX98_FUNCTION_BASE)
elseif(MSVC)
    message(STATUS "Toolset: Microsoft Visual Studio ${MSVC_VERSION}")
    set(QT_VERSION 6.2.2)
    set(MAIN_QT_DIR "d:/Qt/${QT_VERSION}")
    set(MSVC_QT_PATH ${MAIN_QT_DIR}/msvc2019_64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /wd4996 /wd4503 /wd4251 /wd4180 /wd5105 /wd4068")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
elseif(MINGW)
    message(STATUS "Toolset: MinGW GCC")
    add_definitions(-DBOOST_NO_CXX98_FUNCTION_BASE)
endif()
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_DEBUG ON)

set(APP_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(APP_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "APP_ROOT_PATH: ${APP_ROOT_PATH}")
message(STATUS "APP_BUILD_PATH: ${APP_BUILD_PATH}")
