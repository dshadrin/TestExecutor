project( oscar )
set (ProjectDir ${oscar_SOURCE_DIR})
message(STATUS "oscar ProjectDir: " ${ProjectDir})

file( GLOB oscar_SRC ${ProjectDir}/*.c* )
file( GLOB oscar_INC ${ProjectDir}/*.h* )

add_library( oscar STATIC ${oscar_SRC} ${oscar_INC} )
target_include_directories(oscar PRIVATE ${APP_BUILD_PATH}/inc)
if(UNIX)
  target_compile_options(oscar PUBLIC -fPIC)
endif(UNIX)
