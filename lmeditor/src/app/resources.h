#pragma once

#include <filesystem>

#include <boost/dll.hpp>

#include <lmeditor/inspector.h>
#include <lmeditor/map_editor.h>
#include <lmeditor/simulation.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/char_field.h>
#include <lmtk/rect_border.h>

#include "../entity_list/entity_list.h"

namespace lmeditor
{
class editor_app_resources : public lmtk::app_resources
{
  public:
    boost::dll::shared_library game_library;
    std::vector<std::string> simulation_names;
    size_t selected_simulation_index{0};
    create_simulation_fn create_simulation;

    lmtk::font font;
    lmgl::material text_material, border_material, rect_material;

  public:
    editor_app_resources(std::filesystem::path const &project_dir);
    editor_app_resources(editor_app_resources const &) = delete;
    ~editor_app_resources();
    lmtk::rect_border create_active_panel_border(
      lm::point2i const &position,
      lm::size2i const &size);
    void free();
    std::array<float, 4> active_panel_border_colour{1.f, 0.f, 0.f, 1.f};

    lmtk::text_layout create_text_layout(
      std::string const &initial = "",
      lm::point2i position = {0, 0},
      lmtk::ifont const *font = nullptr,
      std::array<float, 3> colour = {1.f, 1.f, 1.f});

    lmtk::char_field create_char_field(
      std::string const &initial = "",
      lm::point2i position = {0, 0},
      lmtk::ifont const *font = nullptr,
      std::array<float, 3> text_colour = {1.f, 1.f, 1.f});
};
} // namespace lmeditor
