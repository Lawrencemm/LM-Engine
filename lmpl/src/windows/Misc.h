#pragma once
#include <string>
#include <windows.h>

namespace lmpl
{
std::string GetCurrentDirectoryMine();
LONGLONG AccurateTime();
LONGLONG PerfFreq();
} // namespace lmpl