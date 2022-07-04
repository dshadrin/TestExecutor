project( utils )
set (ProjectDir ${utils_SOURCE_DIR})
message(STATUS "utils ProjectDir: " ${ProjectDir})

file( GLOB utils_SRC ${ProjectDir}/*.c* )
file( GLOB utils_INC ${ProjectDir}/*.h* )

add_library( utils STATIC ${utils_SRC} ${utils_INC} )
target_include_directories(utils PRIVATE ${APP_BUILD_PATH}/inc ${APP_ROOT_PATH}/inc)
if(UNIX)
  target_compile_options(utils PUBLIC -fPIC)
elseif(MINGW)
  target_link_libraries(utils PUBLIC jsoncpp)
endif()

if(STATIC_PROXY)
  target_compile_definitions(utils PUBLIC LOGGING_DIRECT)
endif()
