cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME "TestExecutor")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
project(${PROJECT_NAME} LANGUAGES CXX C)

set(APP_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Build path: " ${APP_BUILD_PATH})

set(APP_SOURCE_PATH "${CMAKE_SOURCE_DIR}")
message(STATUS "Source path: " ${APP_SOURCE_PATH})

include(config)
include(project)

###
# Boost & platform configuration
###
message(STATUS "Use boost::system")
message(STATUS "Use boost::date_time")
list(APPEND BOOST_LIB_LIST system date_time)

message(STATUS "Use boost::json")
list(APPEND BOOST_LIB_LIST json)

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

message(STATUS "Boost Version: " ${PRJ_BOOST_VERSION})
find_package(Boost ${PRJ_BOOST_VERSION} COMPONENTS ${BOOST_LIB_LIST} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "Boost INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost LIBRARIES: ${Boost_LIBRARIES}")

if(NOT HAVE_STD_FORMAT)
  find_package(fmt CONFIG REQUIRED)
endif()

###
# Qt configuration
###
if(WIN32)
  if(MSVC)
    if(${MSVC_VERSION} STRGREATER "1914")
      set(USED_QT_PATH ${MSVC_QT_PATH})
      set(ENV{CMAKE_PREFIX_PATH} "${USED_QT_PATH}/lib/cmake")
      message(STATUS "USED_QT_PATH: " ${USED_QT_PATH})
    else()
      message(FATAL_ERROR "Unsupported MSVC version")
    endif()
  endif()
  message(STATUS "CMAKE_PREFIX_PATH: " $ENV{CMAKE_PREFIX_PATH})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(STATUS "Use QT version: " ${PRJ_QT})
find_package(${PRJ_QT} ${QT_VERSION} EXACT COMPONENTS Gui Core Widgets REQUIRED)

set(SSH_SRC
  src/SshTypes.cpp
  src/SshSession.cpp
  )

set(SSH_INC
  inc/SshTypes.h
  inc/SshSession.h
  )

set(SOURCES
  ${SSH_SRC}
  src/StdInc.cpp
  src/main.cpp
  src/${PROJECT_NAME}.cpp
  src/AppConfig.cpp
  src/AppConfigDialog.cpp
  src/Console.cpp
  src/QConsole.cpp
  src/Common.cpp
  src/VarEditDialog.cpp
  src/Monitor.cpp
  src/Logger.cpp
  src/ExternalProcess.cpp
)

set(HEADERS
  ${SSH_INC}
  ${APP_BUILD_PATH}/inc/StdInc.h
  inc/${PROJECT_NAME}.h
  inc/AppConfig.h
  inc/AppConfigDialog.h
  inc/Console.h
  inc/QConsole.h
  inc/Common.h
  inc/VarEditDialog.h
  inc/Monitor.h
  inc/Logger.h
  inc/ExternalProcess.h
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
  ui/AppConfig.ui
  ui/VarEdit.ui
)

qt6_add_resources(RESOURCES_R ${RESOURCES})
qt6_wrap_ui(FORM_H ${FORMS})

include_directories(${APP_ROOT_PATH})

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
target_link_libraries(${PROJECT_NAME} ${PRJ_QT}::Core ${PRJ_QT}::Gui ${PRJ_QT}::Widgets ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_ROOT_PATH}/inc ${APP_BUILD_PATH}/inc)

if(MINGW)
  target_link_libraries(${PROJECT_NAME} logclient ssh2 "-static")
elseif(MSVC)
  target_link_libraries(${PROJECT_NAME} oscar utils libssh2)
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} logclient pthread ssh2) #  -lstdc++fs
endif()

if(NOT HAVE_STD_FORMAT)
  target_link_libraries(${PROJECT_NAME} fmt::fmt-header-only)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${DestinationDir})
