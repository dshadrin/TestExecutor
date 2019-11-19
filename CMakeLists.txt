cmake_minimum_required(VERSION 3.11)

include(config.cmake)
project(${PROJECT_NAME} LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

set(APP_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Build path: " ${APP_BUILD_PATH})

set(APP_SOURCE_PATH "${CMAKE_SOURCE_DIR}")
message(STATUS "Source path: " ${APP_SOURCE_PATH})

###
# Boost & platform configuration
###
if(MSVC)
  message(STATUS "Toolset: MSVC")
  add_definitions(-DWINVER=0x0600)
  add_definitions(-D_WIN32_WINNT=0x0600)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /wd4996 /wd4503 /wd4251 /wd4180")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  set(Boost_USE_STATIC_LIBS ON)
elseif(MINGW)
  message(STATUS "Toolset: gcc mingw")
  add_definitions(-DWINVER=0x0502)
  add_definitions(-D_WIN32_WINNT=0x0502)
  add_definitions(-U__STRICT_ANSI__)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_COMPILER "mgw73")
elseif(UNIX)
  message(STATUS "Toolset: gcc unix")
  set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.71 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "Boost LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost COMPILER: ${Boost_COMPILER}")

###
# Qt configuration
###
if(WIN32)
  if(MSVC)
    message(STATUS "MSVC Version: " ${MSVC_VERSION})
    if(${MSVC_VERSION} STRGREATER "1914")
      set(USED_QT_PATH "d:/Qt/5.13.1/msvc2017_64")
    else()
      message(FATAL_ERROR "Unsupported MSVC version")
    endif()
  elseif(MINGW)
    message(STATUS "MINGW toolset" ${MSVC_VERSION})
    set(USED_QT_PATH "d:/Qt/5.13.2/mingw73_64")
  endif()
  set(ENV{CMAKE_PREFIX_PATH} "${USED_QT_PATH}/lib/cmake")
  message(STATUS "CMAKE_PREFIX_PATH: " $ENV{CMAKE_PREFIX_PATH})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Gui Core Widgets REQUIRED)

set(SOURCES
  src/main.cpp
  src/${PROJECT_NAME}.cpp
  src/Config.cpp
  src/ConfigDialog.cpp
  src/console.cpp
  src/VarEditDialog.cpp
)

set(HEADERS
  inc/StdInc.h
  inc/${PROJECT_NAME}.h
  inc/Config.h
  inc/ConfigDialog.h
  inc/console.h
  inc/VarEditDialog.h
)
include_directories(${CMAKE_SOURCE_DIR}/inc)

set(RESOURCES
  res/${PROJECT_NAME}.qrc
)

set(FORMS
  ui/${PROJECT_NAME}.ui
  ui/SessionConfig.ui
  ui/VarEdit.ui
)

qt5_add_resources(RESOURCES_R ${RESOURCES})
qt5_wrap_ui(FORM_H ${FORMS})

###
# App configuration
###
if(MINGW)
#  link_libraries("-static")
endif()
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${FORM_H} ${RESOURCES_R})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets ${Boost_LIBRARIES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${DestinationDir})
