set(PROJECT_NAME "TestExecutor")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")

set(DestinationDir ${CMAKE_SOURCE_DIR}/dist)
message(STATUS "DestinationDir: ${DestinationDir}")

set(MINGW_BOOST_COMPILER "mgw73")
set(MINGW_QT_PATH "d:/Qt/5.13.2/mingw73_64")
set(MSVC_QT_PATH "d:/Qt/5.13.1/msvc2017_64")
set(APP_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(APP_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "APP_ROOT_PATH: ${APP_ROOT_PATH}")
message(STATUS "APP_BUILD_PATH: ${APP_BUILD_PATH}")
                                        