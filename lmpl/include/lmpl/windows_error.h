#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <lmlib/application.h>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>

namespace lmpl
{
class windows_exception : public std::runtime_error
{
    std::string errMsg;

  public:
    windows_exception(std::string &&msg) : std::runtime_error{msg} {}
};

std::string ErrorString(HRESULT errorMessageID);

class throw_failed_hr
{
  public:
    throw_failed_hr(HRESULT hres)
    {
        if (FAILED(hres))
            throw windows_exception(ErrorString(hres));
    }

  private:
};
} // namespace lmpl
#define WinThrowIfFailed(hres)                                                 \
    if (FAILED(hres))                                                          \
    throw MakeExceptionMsg(ErrorString(hres))

template <class CharT, class TraitsT = std::char_traits<CharT>>
class basic_debugbuf : public std::basic_stringbuf<CharT, TraitsT>
{
  public:
    virtual ~basic_debugbuf() { this->sync(); }

#ifdef _MSC_VER
    virtual void _Lock() override { strMutex.lock(); }
    virtual void _Unlock() override { strMutex.unlock(); }
#endif

  protected:
    int sync()
    {
        output_debug_string(this->str().c_str());
        this->str(std::basic_string<CharT>()); // Clear the string buffer

        return 0;
    }

    void output_debug_string(const CharT *text) {}

    mutable std::mutex strMutex;
};

template <>
inline void basic_debugbuf<char>::output_debug_string(const char *text)
{
    ::OutputDebugStringA(text);
}

template <>
inline void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
{
    ::OutputDebugStringW(text);
}

template <class CharT, class TraitsT = std::char_traits<CharT>>
class basic_dostream : public std::basic_ostream<CharT, TraitsT>
{
  public:
    basic_dostream() : std::basic_ostream<CharT, TraitsT>(&buffa)
    {
        this->setf(std::ios::unitbuf);
    }

    ~basic_dostream() {}

  private:
    basic_debugbuf<CharT, TraitsT> buffa{};
};

typedef basic_dostream<char> vsdostream;
typedef basic_dostream<wchar_t> wvsdostream;

extern vsdostream vsdout;
extern vsdostream wvsdout;