#include <stdarg.h>

static int vaspf( char** str, const char* fmt, va_list args )
{
    int size = 0;
    va_list tmpa;

    // copy
    va_copy( tmpa, args );

    // apply variadic arguments to
    // sprintf with format to get size
    size = vsnprintf( NULL, 0, fmt, tmpa );

    // toss args
    va_end( tmpa );

    // return -1 to be compliant if
    // size is less than 0
    if ( size < 0 )
    {
        return -1;
    }

    // alloc with size plus 1 for `\0'
    *str = (char*)malloc( size + 1 );

    // return -1 to be compliant
    // if pointer is `NULL'
    if ( NULL == *str )
    {
        return -1;
    }

    // format string with original
    // variadic arguments and set new size
    size = vsprintf( *str, fmt, args );
    return size;
}

#ifndef HAVE_ASPRINTF
extern "C" int asprintf( char** strp, const char* fmt, ... )
{
    int size = 0;
    va_list args;

    // init variadic arguments
    va_start( args, fmt );

    // format and get size
    size = vaspf( strp, fmt, args );

    // toss args
    va_end( args );

    return size;
}
#endif 

std::string StringFormat( const char* fmt, ... )
{
    char* str = nullptr;
    va_list args;
    // init variadic arguments
    va_start( args, fmt );
    // format and get size
    vaspf( &str, fmt, args );
    // toss args
    va_end( args );

    std::string ret;
    if (str != nullptr)
    {
        ret = str;
        free( str );
    }
    return ret;
}
