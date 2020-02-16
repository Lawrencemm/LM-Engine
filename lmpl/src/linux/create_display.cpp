#include "xcb/xcb_display.h"

namespace lmpl
{

display create_display() { return std::make_unique<lmpl::XcbDisplay>(); }

} // namespace lmpl
