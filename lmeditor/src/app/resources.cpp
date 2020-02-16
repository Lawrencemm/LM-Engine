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
    : project_dir{project_dir},
      inspector_size{lm::size2i{
        window_size.width / 5,
        window_size.height,
      }},
      entity_list_size{inspector_size},
      map_editor_size{lm::size2i{
        window_size.width - inspector_size.width - entity_list_size.width,
        window_size.height,
      }}
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

    auto border_node = function_node<lmgl::material>{
      init_graph, 1, [&](lmgl::material material) {
          active_panel_border =
            std::make_unique<lmtk::rect_border>(lmtk::rect_border{
              renderer.get(),
              material,
              {0, 0},
              {0, 0},
              active_panel_border_colour,
              1.f,
            });
          return continue_msg{};
      }};

    make_edge(border_material_node, border_node);

    using widget_args_type = tuple<lmgl::material, lmtk::ifont const *>;

    auto widget_args_join_node = join_node<widget_args_type>{init_graph};

    lm::make_edge(text_material_node, widget_args_join_node);
    lm::make_edge(font_node, widget_args_join_node);

    auto map_editor_node =
      function_node<widget_args_type>(init_graph, 1, [&](auto inputs) {
          map_editor = create_map_editor(map_editor_init{
            .renderer = renderer.get(),
            .position = {inspector_size.width, 0},
            .size = map_editor_size,
            .selection_outline_colour = std::array{0.7f, 0.5f, 0.5f},
            .text_material = get<0>(inputs),
            .font = get<1>(inputs),
          });
      });

    make_edge(widget_args_join_node, map_editor_node);

    auto inspector_node =
      function_node<widget_args_type>(init_graph, 1, [&](auto inputs) {
          inspector = create_inspector(
            *renderer, get<0>(inputs), get<1>(inputs), inspector_size);
      });

    make_edge(widget_args_join_node, inspector_node);

    root_node.try_put(continue_msg{});
    init_graph.wait_for_all();

    entity_list = std::make_unique<lmeditor::entity_list>(entity_list_init{
      .renderer = *renderer,
      .text_material = text_material,
      .rect_material = rect_material,
      .font = font.get(),
      .position = {inspector_size.width + map_editor_size.width, 0},
      .size = entity_list_size,
    });
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
    active_panel_border->move_resources(renderer.get(), resource_sink);
    map_editor->move_resources(renderer.get(), resource_sink);
    inspector->move_resources(renderer.get(), resource_sink);
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
