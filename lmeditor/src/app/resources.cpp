#include <random>

#include <tbb/flow_graph.h>
#include <tbb/task_scheduler_init.h>

#include <lmeditor/map_editor.h>
#include <lmeditor/trimesh.h>
#include <lmgl/renderer.h>
#include <lmlib/flow_graph.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/rect_border.h>
#include <lmtk/shapes.h>

#include "resources.h"

namespace lmeditor
{
using dep_node = tbb::flow::continue_node<tbb::flow::continue_msg>;

template <typename callable_type>
dep_node create_node(tbb::flow::graph &g, callable_type fn)
{
    return dep_node{
      g,
      [fn = std::move(fn)](auto) {
          fn();
          return tbb::flow::continue_msg{};
      },
    };
}

editor_app_resources::editor_app_resources(
  std::filesystem::path const &project_dir)
{
    using namespace tbb::flow;

    graph init_graph;

    broadcast_node<continue_msg> root_node{init_graph};

    auto game_lib_node = create_node(init_graph, [&] {
        game_library = boost::dll::shared_library{
          (project_dir / "game").c_str(),
          boost::dll::load_mode::append_decorations,
        };
        game_library.get<set_meta_context_fn>("set_meta_context")(
          entt::meta_ctx{});
        game_library.get<reflect_types_fn>("reflect_types")();
        simulation_names =
          game_library.get<list_simulations_fn>("list_simulations")();
        create_simulation =
          game_library.get<create_simulation_fn>("create_simulation");
    });

    make_edge(root_node, game_lib_node);

    auto font_node = continue_node<lmtk::ifont const *>(init_graph, [&](auto) {
        font = font_loader->create_font(lmtk::font_init{
          .renderer = renderer.get(),
          .typeface_name = "Arial",
          .pixel_size = 32,
        });
        return font.get();
    });

    make_edge(root_node, font_node);

    auto text_material_node =
      continue_node<lmgl::material>(init_graph, [&](auto) {
          text_material = lmtk::text_layout::create_material(*renderer);
          return text_material;
      });

    make_edge(root_node, text_material_node);

    auto border_material_node = continue_node<lmgl::material>{
      init_graph, [&](auto) {
          border_material = lmtk::rect_border::create_material(renderer.get());
          return border_material;
      }};

    make_edge(root_node, border_material_node);

    auto rect_material_node = continue_node<lmgl::material>{
      init_graph, [&](auto) {
          rect_material = lmtk::rect::create_material(renderer.get());
          return rect_material;
      }};

    make_edge(root_node, rect_material_node);

    root_node.try_put(continue_msg{});
    init_graph.wait_for_all();
}

editor_app_resources::~editor_app_resources() {}

lmtk::rect_border editor_app_resources::create_active_panel_border(
  lm::point2i const &position,
  lm::size2i const &size)
{
    return lmtk::rect_border{
      renderer.get(),
      border_material,
      position,
      size,
      active_panel_border_colour,
      1.f,
    };
}

void editor_app_resources::free()
{
    resource_sink.add(
      renderer.get(), text_material, border_material, rect_material);
    font->move_resources(renderer.get(), resource_sink);
}

lmtk::char_field editor_app_resources::create_char_field(
  std::string const &initial,
  lm::point2i position,
  lmtk::ifont const *font,
  std::array<float, 3> text_colour)
{
    if (!font)
        font = this->font.get();

    return lmtk::char_field(lmtk::char_field_init{
      .renderer = *renderer,
      .material = text_material,
      font,
      text_colour,
      position,
      initial,
    });
}

lmtk::text_layout editor_app_resources::create_text_layout(
  std::string const &initial,
  lm::point2i position,
  lmtk::ifont const *font,
  std::array<float, 3> colour)
{
    if (!font)
        font = this->font.get();

    return lmtk::text_layout{lmtk::text_layout_init{
      .renderer = *renderer,
      .material = text_material,
      .font = font,
      .colour = {1.f, 1.f, 1.f},
      .position = position,
      .text = initial,
    }};
}
} // namespace lmeditor
