#include "app.h"
#include <fmt/format.h>
#include <lmeditor/component/asset_list.h>
#include <lmeditor/component/command_palette.h>
#include <lmeditor/component/entity_list.h>
#include <lmeditor/component/map_editor.h>
#include <lmeditor/component/player.h>
#include <lmeditor/component/saver.h>
#include <lmeditor/model/creation_time.h>
#include <lmeditor/model/orbital_camera.h>
#include <lmlib/variant_visitor.h>
#include <lmng/animation.h>
#include <lmng/archetype.h>
#include <lmng/logging.h>
#include <lmng/name.h>
#include <lmtk/choice_list.h>
#include <random>
#include <spdlog/spdlog.h>
#include <tbb/task_group.h>
#include <yaml-cpp/yaml.h>

using namespace tbb::flow;

namespace lmeditor
{
editor_app::editor_app(const std::filesystem::path &project_dir)
    : project_dir{project_dir},
      asset_cache{project_dir / "assets"},
      save_map_buffer_node{app_lifetime_graph},
      save_map_node(
        app_lifetime_graph,
        1,
        [&](auto &save_map_msg)
        {
            save_map(
              project_dir / (map_file_project_relative_path + ".lmap"),
              *save_map_msg.map);
        }),
      active_component_border{
        std::make_unique<lmtk::rect_border>(lmtk::rect_border{
          resources.renderer.get(),
          *resources.resource_cache,
          {0, 0},
          {0, 0},
          {1.f, 0.f, 0.f, 1.f},
          1.f,
        })},
      state{gui_state{*this}}
{
    make_edge(save_map_buffer_node, save_map_node);

    YAML::Node project_config =
      YAML::LoadFile((project_dir / "lmproj.yml").string());

    tbb::task_group task_group;

    task_group.run(
      [&]()
      {
          project_plugin.load(
            project_config["editor_plugin_name"].as<std::string>(),
            entt::meta_ctx{});
      });

    auto window_size = resources.window->get_size_client();

    auto inspector_size = lm::size2i{
      window_size.width / 5,
      window_size.height,
    };
    auto map_editor_size = lm::size2i{
      window_size.width - 2 * inspector_size.width,
      window_size.height,
    };

    auto map_editor = map_editor_init{
      map,
      orbital_camera_init{
        .fov = (float)M_PI / 3,
        .aspect = (float)map_editor_size.width / map_editor_size.height,
        .near_clip = 0.1f,
        .far_clip = 1000.f,
        .position = Eigen::Vector3f{0.f, 10.f, -25.f},
        .target = Eigen::Vector3f{0.f, 0.f, 0.f},
      },
      resources.renderer.get(),
      *resources.resource_cache,
      {0, 0},
      map_editor_size,
      std::array{1.f, 0.f, 0.f},
    }();

    auto config_asset_dir = project_config["asset_directory"];

    auto asset_list = asset_list_init{
      .asset_dir = config_asset_dir
                     ? project_dir / config_asset_dir.as<std::string>()
                     : project_dir / "assets",
      .position = {0, 0},
      .size = inspector_size,
      .renderer = resources.renderer.get(),
      .resource_cache = *resources.resource_cache,
    }();

    asset_list->on_select_map().connect<&editor_app::load_map>(this);

    auto inspector = inspector_init{
      .registry = map,
      .renderer = *resources.renderer,
      .resource_cache = *resources.resource_cache,
      .size = inspector_size,
    }();

    auto entity_list = entity_list_init{
      .registry = map,
      .renderer = *resources.renderer,
      .resource_cache = *resources.resource_cache,
      .size = inspector->get_size(),
    }();

    assign_view_key(lmpl::key_code::M, map_editor.get());
    assign_view_key(lmpl::key_code::A, asset_list.get());
    assign_view_key(lmpl::key_code::I, inspector.get());
    assign_view_key(lmpl::key_code::L, entity_list.get());

    visible_components.insert(
      visible_components.begin(),
      {map_editor.get(), asset_list.get(), entity_list.get()});

    component_order.insert(
      component_order.begin(),
      {asset_list.get(), inspector.get(), map_editor.get(), entity_list.get()});

    main_component = map_editor.get();

    components.emplace_back(std::move(map_editor));
    components.emplace_back(std::move(asset_list));
    components.emplace_back(std::move(inspector));
    components.emplace_back(std::move(entity_list));

    refit_visible_components();

    map.on_construct<lmng::name>().connect<&assign_creation_time>();

    task_group.wait();
}

void editor_app::assign_view_key(
  lmpl::key_code code,
  component_interface *pview)
{
    auto [_unused_, was_inserted] = key_code_view_map.try_emplace(code, pview);
    if (!was_inserted)
        throw std::runtime_error{fmt::format(
          "Key {} already bound to another view toggle",
          lmpl::get_keycode_string(code))};
}

lmtk::component_state editor_app::on_event(const lmtk::event &event)
{
    event >>
      lm::variant_visitor{
        [&](lmtk::resize_event const &resize_event)
        { refit_visible_components(); },
        [&](lmtk::quit_event const &)
        {
            for (auto &component : components)
            {
                component->move_resources(resources.resource_sink);
            }

            resources.resource_cache->move_resources(resources.resource_sink);
        },
        [](auto &) {},
      };
    return state >> lm::variant_visitor{
                      [&](auto &state) { return state.handle(*this, event); },
                    };
}

bool editor_app::on_simulation_selected(unsigned selection_index)
{
    selected_simulation_index = selection_index;
    change_state<gui_state>();

    return true;
}

lmtk::component editor_app::create_simulation_selector()
{
    auto selector =
      lmtk::choice_list_init{
        .choices = project_plugin.get_simulation_names(),
        .renderer = resources.renderer.get(),
        .resource_cache = *resources.resource_cache,
      }
        .unique();

    selector->on_selected().connect<&editor_app::on_simulation_selected>(this);

    return std::move(selector);
}

lmtk::component editor_app::create_map_saver()
{
    auto path = map_file_project_relative_path;

    auto saver =
      saver_init{
        .renderer = resources.renderer.get(),
        .resource_cache = *resources.resource_cache,
        .header_text = "Save Map",
        .initial_text = path,
      }
        .unique();

    saver->on_save().connect<&editor_app::on_map_saved>(this);

    return std::move(saver);
}

lmtk::component editor_app::create_command_palette()
{
    return command_palette_init{
      .renderer = *resources.renderer,
      .resource_cache = *resources.resource_cache,
      .commands = visible_components.front()->get_command_descriptions(),
    }
      .unique();
}

lmtk::component editor_app::create_pose_saver(std::string initial_project_path)
{
    auto saver =
      saver_init{
        .renderer = resources.renderer.get(),
        .resource_cache = *resources.resource_cache,
        .header_text = "Save Pose",
        .initial_text = initial_project_path,
      }
        .unique();

    saver->on_save().connect<&editor_app::on_pose_saved>(this);

    return std::move(saver);
}

lmtk::component editor_app::create_pose_loader()
{
    auto is_lpose_file = [](auto &file)
    { return file.path().extension() == ".lpose"; };

    auto proj_relative_no_ext = [&](auto &entry)
    {
        auto with_ext = std::filesystem::relative(entry, project_dir).string();
        return with_ext.substr(
          0, with_ext.size() - std::string{".lpose"}.size());
    };

    std::vector<std::string> pose_paths;
    for (auto &path :
         std::filesystem::recursive_directory_iterator{project_dir})
    {
        if (!is_lpose_file(path))
            continue;

        pose_paths.emplace_back(proj_relative_no_ext(path));
    }

    auto selector =
      lmtk::choice_list_init{
        .choices = pose_paths,
        .renderer = resources.renderer.get(),
        .resource_cache = *resources.resource_cache,
      }
        .unique();

    selector->on_selected().connect<&editor_app::on_pose_selected>(this);

    return std::move(selector);
}

void editor_app::move_current_state_resources()
{
    state >> lm::variant_visitor{
               [&](auto &old_state) {
                   old_state.move_resources(
                     resources.renderer.get(), resources.resource_sink);
               }};
}

lmtk::component editor_app::create_player()
{
    return player_init{
      .renderer = resources.renderer.get(),
      .map = map,
      .project_plugin = project_plugin,
      .simulation_index = selected_simulation_index,
      .asset_cache = asset_cache,
      .position = {0, 0},
      .size = resources.window->get_size_client(),
    }
      .unique();
}

void editor_app::on_construct_any(
  entt::registry &registry,
  entt::entity entity,
  entt::meta_type const &meta_type)
{
    if (&registry == &map)
    {
        SPDLOG_INFO("Serialising map async");
        save_map_async();
    }
}

void editor_app::on_update_any(
  entt::registry &registry,
  entt::entity entity,
  entt::meta_type const &meta_type)
{
    if (&registry == &map)
    {
        SPDLOG_INFO("Serialising map async");
        save_map_async();
    }
}

void editor_app::on_destroy_any(
  entt::registry &registry,
  entt::entity entity,
  entt::meta_type const &meta_type)
{
    if (&registry == &map)
    {
        SPDLOG_INFO("Serialising map async");
        save_map_async();
    }
}

void editor_app::main()
try
{
    enter();
}
catch (std::exception const &e)
{
    std::cerr << e.what() << std::endl;
    throw std::runtime_error{
      fmt::format("App main function failed with exception {}", e.what())};
}
} // namespace lmeditor
