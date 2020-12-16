#pragma once

#include <ctime>
#include <string>

// Function for timestamp operations
namespace TS
{
void TimestampAdjust(timespec& ts, long deltaFromNowMcs);
void ConvertTimestamp(timespec tv, tm* tmStruct, long* us, long deltaFromNowMcs = 0);
struct timespec GetTimestamp();
std::string GetTimestampStr();
std::string GetTimestampStr(const timespec& tv);
std::string GetTimestampStr(struct tm& tmStruct, long us);
}