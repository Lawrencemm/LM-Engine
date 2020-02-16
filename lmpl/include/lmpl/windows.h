#pragma once
#include <windows.h>

#include "lmpl.h"

namespace lmpl
{
class iwindows_window : public iwindow
{
public:
  virtual HWND get_hwnd() = 0;
};
} // namespace lmpl
