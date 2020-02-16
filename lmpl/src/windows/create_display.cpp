#include <lmlib/application.h>
#include <lmpl/lmpl.h>

#include "windows_display.h"

namespace lmpl
{
display create_display()
{
    auto p_display = std::make_unique<windows_display>();
    p_display->init();
    return p_display;
}

} // namespace lmpl