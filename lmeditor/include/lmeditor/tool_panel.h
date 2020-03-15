#pragma once

#include "command.h"
#include <lmtk/iwidget.h>

namespace lmeditor
{
class editor_app;

class itool_panel : public lmtk::iwidget
{
  public:
    virtual itool_panel &
      add_to_frame(lmgl::iframe *frame, editor_app const &app) = 0;
    virtual std::vector<command_description> get_command_descriptions() = 0;
};
} // namespace lmeditor
