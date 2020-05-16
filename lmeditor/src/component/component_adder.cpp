#include "lmeditor/component/inspector.h"

#include <fmt/format.h>

namespace lmeditor
{
inspector::add_state inspector::create_add_state()
{
    lm::point2i pos{size.width, 0};
    lmtk::rect background{
      renderer,
      background_material,
      pos,
      size,
      {0.f, 0.f, 0.f, 1.f},
    };
    lmtk::text_layout title_line{
      lmtk::text_layout_init{
        renderer,
        text_material,
        font,
        {1.f, 1.f, 1.f},
        pos,
        "Add Component",
      },
    };

    std::vector<add_state::entry> entries;
    int height{32};
    int index{0};
    entt::resolve([&](entt::meta_type const &component_type) {
        if (!component_type.prop(entt::hashed_string{"is_component"}))
            return;

        auto entry_text =
          fmt::format("  {}", lmng::get_type_name(component_type));
        if (index++ == 0)
            entry_text[1] = '*';

        lmtk::text_layout line{
          lmtk::text_layout_init{
            renderer,
            text_material,
            font,
            {1.f, 1.f, 1.f},
            {pos.x, pos.y + height},
            entry_text,
          },
        };
        entries.emplace_back(add_state::entry{component_type, std::move(line)});
        height += 32;
    });

    return add_state{
      .background = std::move(background),
      .title_line = std::move(title_line),
      .entries = std::move(entries),
    };
}

void inspector::add_state::render(lmgl::iframe *frame)
{
    background.add_to_frame(frame);
    title_line.render(frame);
    for (auto &data : entries)
    {
        data.line.render(frame);
    }
}

bool inspector::add_state::handle(state_handle_args const &args)
{
    return args.input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_msg) {
                 switch (key_down_msg.key)
                 {
                 case lmpl::key_code::I:
                     return move_selection(
                       args.inspector, -1, args.resource_sink);

                 case lmpl::key_code::K:
                     return move_selection(
                       args.inspector, 1, args.resource_sink);

                 case lmpl::key_code::Enter:
                 {
                     auto meta_type = selection().type;
                     move_resources(
                       &args.inspector.renderer, args.resource_sink);
                     args.inspector.state = select_state{};
                     args.event_handler(inspector_added_component{meta_type});
                     return true;
                 }

                 default:
                     return false;
                 }
             },
             [](auto) { return false; },
           };
}

bool inspector::add_state::move_selection(
  inspector &inspector,
  int movement,
  lmtk::resource_sink &resource_sink)
{
    int new_pos = std::max(
      std::min(selected_entry_index + movement, (int)entries.size() - 1), 0);

    if (selected_entry_index == new_pos)
        return false;

    auto &entry = selection();
    entry.line.set_text(
      inspector.renderer,
      inspector.font,
      fmt::format("  {}", lmng::get_type_name(entry.type)),
      resource_sink);

    selected_entry_index = new_pos;
    auto &new_entry = selection();
    new_entry.line.set_text(
      inspector.renderer,
      inspector.font,
      fmt::format(" *{}", lmng::get_type_name(new_entry.type)),
      resource_sink);
    return true;
}

void inspector::add_state::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    background.move_resources(renderer, resource_sink);
    title_line.move_resources(renderer, resource_sink);
    for (auto &entry : entries)
        entry.line.move_resources(renderer, resource_sink);
}
} // namespace lmeditor
