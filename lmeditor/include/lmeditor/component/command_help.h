#pragma once

#include "lmeditor/model/command.h"
#include <lmgl/lmgl.h>
#include <lmtk/char_field.h>
#include <lmtk/component.h>
#include <lmtk/table.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct command_help_init;
class command_help : public lmtk::component_interface
{
  public:
    explicit command_help(lmeditor::command_help_init const &init);

    command_help &add_to_frame(lmgl::iframe *frame) override;
    bool handle(lmtk::input_event const &input_event) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    command_help &set_rect(lm::point2i position, lm::size2i size) override;

    command_help &move_resources(lmgl::resource_sink &resource_sink) override;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache) override;

  private:
    lmtk::char_field filter;
    std::vector<std::array<lmtk::text_layout, 3>> rows;
    std::vector<command_description> commands;

    lm::point2i get_table_origin();
};

struct command_help_init
{
    lmgl::irenderer &renderer;
    lmtk::resource_cache const &resource_cache;
    std::vector<command_description> commands;

    lmtk::component unique();
};
} // namespace lmeditor
