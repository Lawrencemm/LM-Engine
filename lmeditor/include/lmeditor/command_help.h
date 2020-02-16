#pragma once

#include <lmgl/lmgl.h>
#include <lmtk/char_field.h>
#include <lmtk/iwidget.h>
#include <lmtk/table.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct command_description
{
    std::string name, key, context;
};

struct command_help_init
{
    lmgl::irenderer &renderer;
    lmgl::material material;
    lmtk::ifont const *font;
    std::vector<command_description> commands;
};

class command_help : public lmtk::iwidget
{
  public:
    iwidget &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    command_help(lmeditor::command_help_init const &init);
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;

    lmtk::char_field filter;
    std::vector<std::array<lmtk::text_layout, 3>> rows;
    std::vector<command_description> commands;

    bool handle(
      lmtk::input_event const &input_event,
      lmgl::irenderer *renderer,
      lmtk::ifont const *font,
      lmtk::resource_sink &resource_sink);
    lm::point2i get_table_origin();
};
} // namespace lmeditor
