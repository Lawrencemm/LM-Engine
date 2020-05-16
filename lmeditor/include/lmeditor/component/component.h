#pragma once

#include <lmeditor/model/command.h>
#include <lmtk/component.h>

namespace lmeditor
{
class component_interface : public lmtk::component_interface
{
  public:
    virtual std::vector<command_description> get_command_descriptions() = 0;
};

using component = lm::reference<component_interface>;
} // namespace lmeditor
