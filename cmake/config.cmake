set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

include(CheckIncludeFileCXX)
# use boost filesystem because it required for boost::process
# check_include_file_cxx("filesystem" HAVE_STD_FILESYSTEM)
check_include_file_cxx("cstdint" HAVE_STD_INT)
check_include_file_cxx("thread" HAVE_STD_THREAD)
check_include_file_cxx("chrono" HAVE_STD_CHRONO)
# use boost regex because it required for boost::algorithm::string
check_include_file_cxx("regex" HAVE_STD_REGEX)

check_include_file_cxx("format" HAVE_STD_FORMAT)
if(HAVE_STD_FORMAT)
  message(STATUS "Standard <format> used")
else()
  message(STATUS "FMT library used")
endif()

# use boost optional because required optional reference objects
# check_include_file_cxx("optional" HAVE_STD_OPTIONAL)
if(HAVE_STD_OPTIONAL)
  message(STATUS "Standard <optional> used")
else()
  message(STATUS "Boost <optional> used")
endif()


configure_file(inc/StdInc.h.in inc/StdInc.h @ONLY)
