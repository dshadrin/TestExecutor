/*
* Copyright (C) 2022 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once

#include <source_location>

namespace util::dex
{

template<class _ExceptionType>
inline void ThrowException( std::string_view message, std::source_location location = std::source_location::current() )
{
    throw _ExceptionType( fmt::format( FMT_STRING( "{:s} ({:d})\n{:s}\n{:s}" ), location.file_name(), location.line(), location.function_name(), message ) );
}

}