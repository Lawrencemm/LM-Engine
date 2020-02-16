#pragma once

#include "app.h"
#include "resources.h"
#include <lmtk/char_field.h>
#include <lmtk/iwidget.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct map_saver : public lmtk::iwidget
{
    lmtk::text_layout header;
    lmtk::char_field field;

    map_saver(editor_app_resources &resources, std::string const &initial);

    static bool handle(
      editor_app &app,
      lmtk::iwidget *widget,
      lmtk::input_event const &input_event);

    lmtk::iwidget &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override { return lm::size2i(); }
    lm::point2i get_position() override { return lm::point2i(); }
    lmtk::iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    lmtk::iwidget &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;
};
} // namespace lmeditor
