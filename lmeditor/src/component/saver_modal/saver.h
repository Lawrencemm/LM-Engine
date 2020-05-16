#pragma once

#include <lmeditor/component/saver.h>
#include <lmtk/char_field.h>
#include <lmtk/text_layout.h>
#include <lmtk/widget.h>

namespace lmeditor
{
class editor_app_resources;

struct saver : public saver_interface
{
    entt::sigh<bool(std::string const &)> save_signal;
    lmtk::text_layout header;
    lmtk::char_field field;

    explicit saver(saver_init const &init);

    saver &add_to_frame(lmgl::iframe *frame) override;

    lm::size2i get_size() override { return lm::size2i(); }

    lm::point2i get_position() override { return lm::point2i(); }

    saver &set_rect(lm::point2i position, lm::size2i size) override;

    saver &move_resources(lmgl::resource_sink &resource_sink) override;

    bool handle(const lmtk::input_event &input_event) override;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) override;

    entt::sink<bool(const std::string &)> on_save() override;
};
} // namespace lmeditor
