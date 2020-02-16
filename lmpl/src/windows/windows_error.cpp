#include <iostream>

#include <lmpl/windows_error.h>

namespace lmpl
{
std::string ErrorString(HRESULT errorMessageID)
{
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      errorMessageID,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer,
      0,
      NULL);
    std::string message(messageBuffer, size);
    while (isspace(message.back()))
    {
        message.resize(message.size() - 1);
    }
    // Free the buffer.
    LocalFree(messageBuffer);
    return message;
}
} // namespace lmpl

class RedirectCerr
{
    basic_debugbuf<char> buffa;
    basic_debugbuf<char> buffb;

  public:
    RedirectCerr()
    {
        std::cerr.rdbuf(&buffa);
        std::cout.rdbuf(&buffb);
    }
};

#ifdef _MSC_VER
RedirectCerr cerrRedirector;
#endif

vsdostream vsdout;
vsdostream wvsdout;