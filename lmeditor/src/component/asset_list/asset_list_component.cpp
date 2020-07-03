#include "asset_list_component.h"
#include <lmlib/variant_visitor.h>
#include <lmtk/vlayout.h>

namespace lmeditor
{
component asset_list_init::operator()()
{
    return std::make_unique<asset_list_component>(*this);
}

std::vector<lmtk::text_layout> create_entries(
  std::filesystem::path const &asset_dir,
  lm::point2i position,
  lmgl::irenderer *renderer,
  lmtk::resource_cache const &resource_cache)
{
    std::vector<lmtk::text_layout> line_layouts;

    auto layout_factory = lmtk::text_layout_factory{
      *renderer, resource_cache, {1.f, 1.f, 1.f}, position};

    std::function<void(std::filesystem::directory_entry, unsigned)> add_subdir;

    add_subdir =
      [&](std::filesystem::directory_entry const &entry, unsigned level) {
          for (auto const &sub_entry :
               std::filesystem::directory_iterator{entry})
          {
              line_layouts.emplace_back(
                layout_factory.create(sub_entry.path().stem().string()));

              auto old_pos = line_layouts.back().position;
              old_pos.x += level * 15;
              line_layouts.back().set_position(old_pos);

              if (sub_entry.is_directory())
                  add_subdir(sub_entry, level + 1);
          }
      };

    for (auto const &entry : std::filesystem::directory_iterator{asset_dir})
    {
        line_layouts.emplace_back(
          layout_factory.create(entry.path().stem().string()));
        if (entry.is_directory())
            add_subdir(entry, 1);
    }

    lmtk::layout_vertical(lmtk::vertical_layout{position.y, 12}, line_layouts);

    return std::move(line_layouts);
}

asset_list_component::asset_list_component(const asset_list_init &init)
    : asset_dir{init.asset_dir},
      position{init.position},
      size{init.size},
      selection_background{
        *init.renderer,
        init.resource_cache,
        init.position,
        {0, 0},
        {0.f, 0.f, 0.f, 1.f},
      },
      line_layouts{
        create_entries(asset_dir, position, init.renderer, init.resource_cache)}
{
}

std::vector<command_description>
  asset_list_component::get_command_descriptions()
{
    return std::vector<command_description>();
}

bool asset_list_component::handle(const lmtk::input_event &input_event)
{
    return input_event >> lm::variant_visitor{
                            [&](lmtk::key_down_event const &key_down_message) {
                                return handle_key_down(key_down_message);
                            },
                            [](auto &) { return false; },
                          };
}

lmtk::component_interface &asset_list_component::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  const lmtk::resource_cache &resource_cache,
  const lmtk::input_state &input_state)
{
    for (auto &layout : line_layouts)
        layout.move_resources(resource_sink);

    line_layouts.clear();

    line_layouts =
      create_entries(asset_dir, position, renderer, resource_cache);

    return *this;
}

void asset_list_component::update_selection_background()
{
    if (line_layouts.empty())
        return;

    lmtk::text_layout &line = line_layouts[selected_index];

    selection_background.set_rect(
      {position.x, line.position.y}, {size.width, line.get_size().height});
}

bool asset_list_component::add_to_frame(lmgl::iframe *frame)
{
    update_selection_background();
    selection_background.add_to_frame(frame);
    for (auto &layout : line_layouts)
        layout.render(frame);

    return false;
}

lm::size2i asset_list_component::get_size() { return size; }

lm::point2i asset_list_component::get_position() { return position; }

lmtk::widget_interface &
  asset_list_component::set_rect(lm::point2i position, lm::size2i size)
{
    this->position = position;
    this->size = size;
    return *this;
}

lmtk::widget_interface &
  asset_list_component::move_resources(lmgl::resource_sink &resource_sink)
{
    for (auto &layout : line_layouts)
        layout.move_resources(resource_sink);

    selection_background.move_resources(resource_sink);

    return *this;
}

bool asset_list_component::handle_key_down(const lmtk::key_down_event &event)
{

    switch (event.key)
    {
    case lmpl::key_code::I:
        return move_selection(-1);

    case lmpl::key_code::K:
        return move_selection(1);

    default:
        return false;
    }
}

bool asset_list_component::move_selection(int movement)
{
    if (movement < 0 && selected_index == 0)
        return false;

    if (movement > 0 && selected_index == line_layouts.size() - 1)
        return false;

    selected_index += movement;

    return true;
}
} // namespace lmeditor
