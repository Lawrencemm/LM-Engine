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

bool saver::add_to_frame(lmgl::iframe *frame)
{
    header.render(frame);
    field.add_to_frame(frame);
    return false;
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

bool saver::handle(const lmtk::input_event &input_event)
{
    bool saved{false};

    bool someone_is_dirty =
      input_event >> lm::variant_visitor{
                       [&](lmtk::key_down_event const &key_down_event) {
                           if (key_down_event.key == lmpl::key_code::Enter)
                           {
                               saved = true;
                               return lmtk::collect_dirty_signal(
                                 save_signal, field.get_value());
                           }
                           return false;
                       },
                       [](auto &) { return false; },
                     };

    if (saved)
        return someone_is_dirty;

    return field.handle(input_event);
}

lmtk::component_interface &saver::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state)
{
    field.update(renderer, resource_sink, resource_cache, input_state);
    return *this;
}

entt::sink<bool(const std::string &)> saver::on_save() { return save_signal; }
} // namespace lmeditor
