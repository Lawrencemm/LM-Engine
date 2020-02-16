#include <vector>

#include "Misc.h"

namespace lmpl
{
std::string GetCurrentDirectoryMine()
{
    auto size = GetCurrentDirectoryA(0, nullptr);
    std::vector<char> output(size, '\0');
    GetCurrentDirectoryA(size, output.data());
    return std::string{&output[0]};
}

LONGLONG AccurateTime()
{
    LARGE_INTEGER out;
    QueryPerformanceCounter(&out);
    return out.QuadPart;
}

LONGLONG PerfFreq()
{
    LARGE_INTEGER out;
    QueryPerformanceFrequency(&out);
    return out.QuadPart;
}
} // namespace lmpl
