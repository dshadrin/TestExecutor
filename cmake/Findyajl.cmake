#
# Findpal module
#
# Sets the following variables:
#
#  - YAIL_FOUND         to TRUE if libpal or libsdk_pal is found
#
#  - YAIL_LIBRARIES     to path to library to link
#
#  - YAIL_INCLUDE_DIRS  to directory containing pal include files
#
#  - YAIL_DEPENDENCIES  to the list of dependencies to build before linking the pal library
#                      If dependencies are not required, the variable will be removed
#                      So, in CMakeLists.txt file adding dependencies should look like this:
#                      if(PAL_DEPENDENCIES)
#                          add_dependencies(<YOUR_PROJECT_NAME> ${PAL_DEPENDENCIES})
#                      endif()
#

message(STATUS "Yajl Library handle")
include(ExternalProject)
find_library(YAJL_LIBRARIES
    NAMES
        yajl_s
    PATHS
        ${THIRD_PARTY_LIB_PATH}
    NO_DEFAULT_PATH
)
if(YAIL_LIBRARIES)
    message(STATUS "Yajl is found: \"${YAJL_LIBRARIES}\"")
    set(YAJL_FOUND true)
else()
    if(NOT TARGET YAJLLIB)
        if(MINGW)
            # -Wno-format is for MinGW:
            # its GCC uses WIN libraries which don't support C99 standard
            # Error: printf("%llu") causes compiler shout due to "llu"
            set(MINGW_WORKAROUND "-std=gnu++11 -Wno-format")
        endif()

        message (STATUS "Precompiled YAJL library is not found! Build preparation....")
        ExternalProject_Add(YAJLLIB
            STAMP_DIR ${APP_BUILD_PATH}/yajl/.stamps
            BINARY_DIR ${APP_BUILD_PATH}/yajl/.build
            PREFIX "${APP_ROOT_PATH}/yajl"
            SOURCE_DIR "${APP_ROOT_PATH}/yajl"
            INSTALL_DIR "${THIRD_PARTY_LIB_PATH}"
            CMAKE_ARGS
                "-DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}"
                "-DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}"
                "-DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_LIB_PATH}"
                "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            STEP_TARGETS build install
        )
    endif()
    set(YAJL_FOUND true)
    set(YAJL_LIBRARIES yajl)
    set(YAJL_DEPENDENCIES YAJLLIB)
endif()

set(YAJL_INCLUDE_DIRS ${YAJL_INCLUDE_DIRS})
