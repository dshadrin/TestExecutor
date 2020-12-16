INCLUDE(CheckIncludeFileCXX)
if(NOT UNIX)
check_include_file_cxx("filesystem" HAVE_STD_FILESYSTEM)
endif()
check_include_file_cxx("cstdint" HAVE_STD_INT)
check_include_file_cxx("thread" HAVE_STD_THREAD)
check_include_file_cxx("chrono" HAVE_STD_CHRONO)
if(NOT MSVC)
check_include_file_cxx("regex" HAVE_STD_REGEX)
endif()

include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(asprintf "stdio.h" HAVE_ASPRINTF)

configure_file(inc/StdInc.h.in inc/StdInc.h @ONLY)
                                         