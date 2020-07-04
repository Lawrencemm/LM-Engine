#pragma once

#include <lmeditor/component/asset_list.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class asset_list_component : public asset_list_interface
{
  public:
    explicit asset_list_component(asset_list_init const &init);

    std::vector<command_description> get_command_descriptions() override;

    bool handle(const lmtk::input_event &input_event) override;

    lmtk::component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      const lmtk::resource_cache &resource_cache,
      const lmtk::input_state &input_state) override;

    bool add_to_frame(lmgl::iframe *frame) override;

    lm::size2i get_size() override;
    lm::point2i get_position() override;

    widget_interface &set_rect(lm::point2i position, lm::size2i size) override;

    widget_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

    entt::sink<bool(const std::filesystem::path &)> on_select_map() override;

  private:
    std::filesystem::path const asset_dir;
    lm::point2i position;
    lm::size2i size;
    unsigned selected_index{0};
    lmtk::rect selection_background;
    std::vector<lmtk::text_layout> line_layouts;
    std::vector<std::filesystem::path> entries;
    entt::sigh<bool(std::filesystem::path const &)> map_selected_signal;

    void update_selection_background();
    bool handle_key_down(const lmtk::key_down_event &event);
    bool move_selection(int movement);
    void create_entries(
      lmgl::irenderer *renderer,
      lmtk::resource_cache const &resource_cache);
};
} // namespace lmeditor
