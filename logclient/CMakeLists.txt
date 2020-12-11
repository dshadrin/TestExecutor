project( logclient )
set (ProjectDir ${logclient_SOURCE_DIR})
message(STATUS "logclient ProjectDir: " ${ProjectDir})

file( GLOB logclient_SRC ${ProjectDir}/*.c* )
file( GLOB logclient_INC ${ProjectDir}/*.h* )

add_library( logclient STATIC ${logclient_SRC} ${logclient_INC} )
target_link_libraries(logclient oscar utils Qt6::Core Qt6::Gui Qt6::Widgets)
target_include_directories(logclient PRIVATE ${APP_BUILD_PATH}/inc)

if(UNIX)
  target_compile_options(logclient PUBLIC -fPIC)
endif(UNIX)

if(MINGW)
  target_link_libraries(logclient ws2_32)    
endif()
