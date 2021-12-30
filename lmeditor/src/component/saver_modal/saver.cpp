#include "saver.h"
#include <entt/entt.hpp>
#include <lmeditor/component/saver.h>
#include <lmlib/variant_visitor.h>

namespace lmeditor
{
lm::reference<saver_interface> saver_init::unique()
{
    return std::make_unique<saver>(*this);
}

saver::saver(saver_init const &init)
    : header{lmtk::text_layout_init{
        .renderer = *init.renderer,
        .resource_cache = init.resource_cache,
        .colour = {1.f, 1.f, 1.f},
        .position = {0, 0},
        .text = init.header_text,
      }},
      field{lmtk::char_field_init{
        .renderer = *init.renderer,
        .resource_cache = init.resource_cache,
        .text_colour = {1.f, 1.f, 1.f},
        .position = {0, header.get_size().height + 15},
        .initial = init.initial_text,
      }}
{
}

saver &saver::move_resources(lmgl::resource_sink &resource_sink)
{
    header.move_resources(resource_sink);
    field.move_resources(resource_sink);
    return *this;
}

saver &saver::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
    return *this;
}

bool saver::handle(const lmtk::event &event)
{
    bool saved{false};

    bool someone_is_dirty =
      event >> lm::variant_visitor{
                 [&](lmtk::key_down_event const &key_down_event)
                 {
                     if (key_down_event.key == lmpl::key_code::Enter)
                     {
                         saved = true;
                         return lmtk::collect_dirty_signal(
                           save_signal, field.get_value());
                     }
                     return false;
                 },
                 [&](lmtk::draw_event const &draw_event)
                 {
                     header.render(&draw_event.frame);
                     field.handle(draw_event);
                     return false;
                 },
                 [](auto &) { return false; },
               };

    if (saved)
        return someone_is_dirty;

    return field.handle(event);
}

entt::sink<bool(const std::string &)> saver::on_save() { return save_signal; }
} // namespace lmeditor
