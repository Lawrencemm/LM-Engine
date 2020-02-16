#pragma once

#include "command_help.h"

#include <lmtk/lmtk.h>

namespace lmeditor
{
class itool_panel : public lmtk::iwidget
{
  public:
    virtual std::vector<command_description> get_command_descriptions() = 0;
};
} // namespace lmeditor
