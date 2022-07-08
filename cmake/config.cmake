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
check_include_file_cxx("ranges" HAVE_STD_RANGE)

# use boost optional because required optional reference objects
# check_include_file_cxx("optional" HAVE_STD_OPTIONAL)


configure_file(inc/StdInc.h.in inc/StdInc.h @ONLY)
