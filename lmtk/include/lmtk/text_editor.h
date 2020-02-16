#pragma once

#include "text_layout.h"

namespace lmtk
{
class text_editor
{
  public:
    explicit text_editor(std::string initial);
    bool handle(input_event const &event);

    std::string text;
};
} // namespace lmtk
