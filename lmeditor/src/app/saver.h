#pragma once

#include <lmtk/char_field.h>
#include <lmtk/iwidget.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class editor_app_resources;

struct saver : public lmtk::iwidget
{
    lmtk::text_layout header;
    lmtk::char_field field;

    saver(
      editor_app_resources &resources,
      std::string const &header,
      std::string const &initial);

    saver &add_to_frame(lmgl::iframe *frame);
    lm::size2i get_size() override { return lm::size2i(); }
    lm::point2i get_position() override { return lm::point2i(); }
    saver &set_rect(lm::point2i position, lm::size2i size) override;
    saver &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;
};
} // namespace lmeditor
