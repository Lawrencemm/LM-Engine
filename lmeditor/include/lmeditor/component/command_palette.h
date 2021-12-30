#pragma once

#include "lmeditor/model/command.h"
#include <lmgl/lmgl.h>
#include <lmtk/char_field.h>
#include <lmtk/component.h>
#include <lmtk/table.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct command_palette_init;
class command_palette : public lmtk::component_interface
{
  public:
    explicit command_palette(lmeditor::command_palette_init const &init);

    bool handle(lmtk::event const &event) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    command_palette &set_rect(lm::point2i position, lm::size2i size) override;

    command_palette &
      move_resources(lmgl::resource_sink &resource_sink) override;

  private:
    bool draw(const lmtk::draw_event &draw_event);

  private:
    lmtk::char_field filter;
    std::array<lmtk::text_layout, 3> table_header;
    std::vector<std::array<lmtk::text_layout, 3>> rows;
    std::vector<std::pair<size_t, double>> sorted_rows;
    std::vector<command_description> commands;

    lm::point2i get_table_origin();
};

struct command_palette_init
{
    lmgl::irenderer &renderer;
    lmtk::resource_cache const &resource_cache;
    std::vector<command_description> commands;

    lmtk::component unique();
};
} // namespace lmeditor
