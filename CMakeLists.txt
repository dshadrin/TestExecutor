cmake_minimum_required(VERSION 3.19)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(project)
project(${PROJECT_NAME} LANGUAGES CXX C)

set(APP_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Build path: " ${APP_BUILD_PATH})

set(APP_SOURCE_PATH "${CMAKE_SOURCE_DIR}")
message(STATUS "Source path: " ${APP_SOURCE_PATH})

include(config)

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
#  add_definitions(-DWINVER=0x0502)
#  add_definitions(-D_WIN32_WINNT=0x0502)
  add_definitions(-D__STRICT_ANSI__)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_COMPILER ${MINGW_BOOST_COMPILER})
elseif(UNIX)
  message(STATUS "Toolset: gcc unix")
  set(Boost_USE_STATIC_LIBS ON)
endif()
set(Boost_USE_MULTITHREADED ON)

list(APPEND BOOST_LIB_LIST system date_time)
if(NOT HAVE_STD_FILESYSTEM)
  message(STATUS "Use boost::filesystem")
  list(APPEND BOOST_LIB_LIST filesystem)
endif()
if(NOT HAVE_STD_REGEX)
  message(STATUS "Use boost::regex")
  list(APPEND BOOST_LIB_LIST regex)
endif()
if(NOT HAVE_STD_THREAD)
  message(STATUS "Use boost::thread")
  list(APPEND BOOST_LIB_LIST thread)
endif()

find_package(Boost ${PRJ_BOOST_VERSION} COMPONENTS ${BOOST_LIB_LIST} REQUIRED)
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
    set(USED_QT_PATH ${MSVC_QT_PATH})
  else()
    message(FATAL_ERROR "Unsupported MSVC version")
  endif()
elseif(MINGW)
  message(STATUS "MINGW toolset gcc")
  set(USED_QT_PATH ${MINGW_QT_PATH})
elseif(UNIX)
  message(STATUS "UNIX toolset gcc")
  set(USED_QT_PATH ${UNIX_QT_PATH})
endif()
set(ENV{CMAKE_PREFIX_PATH} "${USED_QT_PATH}/lib/cmake")
message(STATUS "CMAKE_PREFIX_PATH: " $ENV{CMAKE_PREFIX_PATH})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(STATUS "Use QT version: " ${PRJ_QT})
find_package(${PRJ_QT} COMPONENTS Gui Core Widgets REQUIRED)

set(SSH_SRC
  src/ssh/SshTypes.cpp
  src/ssh/SshSession.cpp
  )

set(SSH_INC
  src/ssh/SshTypes.h
  src/ssh/SshSession.h
  )

set(SOURCES
  ${SSH_SRC}
  src/StdInc.cpp
  src/main.cpp
  src/${PROJECT_NAME}.cpp
  src/Config.cpp
  src/ConfigDialog.cpp
  src/Console.cpp
  src/QConsole.cpp
  src/Common.cpp
  src/VarEditDialog.cpp
  src/Monitor.cpp
  src/Logger.cpp
)

set(HEADERS
  ${SSH_INC}
  ${APP_BUILD_PATH}/inc/StdInc.h
  inc/${PROJECT_NAME}.h
  inc/Config.h
  inc/ConfigDialog.h
  inc/Console.h
  inc/QConsole.h
  inc/Common.h
  inc/VarEditDialog.h
  inc/Monitor.h
  inc/Logger.h
)

if(MSVC)
  list(APPEND HEADERS
    logclient/LogClient.h
    logclient/QLogInterface.h
    )
  list(APPEND SOURCES
    logclient/LogClient.cpp
    )
endif()

set(RESOURCES
  res/${PROJECT_NAME}.qrc
)

set(FORMS
  ui/${PROJECT_NAME}.ui
  ui/SessionConfig.ui
  ui/VarEdit.ui
)

if(UNIX)
  qt5_add_resources(RESOURCES_R ${RESOURCES})
  qt5_wrap_ui(FORM_H ${FORMS})
else()
  qt6_add_resources(RESOURCES_R ${RESOURCES})
  qt6_wrap_ui(FORM_H ${FORMS})
endif()

include_directories(${APP_ROOT_PATH})

add_subdirectory(libssh2)
add_subdirectory(oscar)
add_subdirectory(utils)
if(NOT MSVC)
  add_subdirectory(logclient)
endif()

###
# App configuration
###
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${FORM_H} ${RESOURCES_R})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${APP_BUILD_PATH}/inc/StdInc.h)
target_link_libraries(${PROJECT_NAME} ${PRJ_QT}::Core ${PRJ_QT}::Gui ${PRJ_QT}::Widgets libssh2 ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_ROOT_PATH}/inc ${APP_BUILD_PATH}/inc)

if(MINGW)
  target_link_libraries(${PROJECT_NAME} logclient "-static")
elseif(MSVC)
  target_link_libraries(${PROJECT_NAME} oscar utils)
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} logclient -lpthread) #  -lstdc++fs
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${DestinationDir})
