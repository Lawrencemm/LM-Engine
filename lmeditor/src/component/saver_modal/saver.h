#pragma once

#include <lmeditor/component/saver.h>
#include <lmtk/char_field.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct saver : public saver_interface
{
    entt::sigh<bool(std::string const &)> save_signal;
    lmtk::text_layout header;
    lmtk::char_field field;

    explicit saver(saver_init const &init);

    lm::size2i get_size() override { return lm::size2i(); }

    lm::point2i get_position() override { return lm::point2i(); }

    saver &set_rect(lm::point2i position, lm::size2i size) override;

    saver &move_resources(lmgl::resource_sink &resource_sink) override;

    lmtk::component_state handle(const lmtk::event &event) override;

    entt::sink<bool(const std::string &)> on_save() override;
};
} // namespace lmeditor
