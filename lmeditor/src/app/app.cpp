#include "app.h"
#include "saver.h"
#include <boost/rational.hpp>
#include <lmeditor/map_editor.h>
#include <lmeditor/save_load_pose.h>
#include <lmengine/name.h>
#include <lmlib/variant_visitor.h>
#include <lmtk/text_line_selector.h>
#include <random>
#include <range/v3/view.hpp>

using namespace tbb::flow;

namespace lmeditor
{
editor_app::editor_app(const std::filesystem::path &project_dir)
    : project_dir{project_dir},
      resources{project_dir},
      flow_graph(
        resources,
        [&](auto &ev) { return on_input_event(ev); },
        [&](auto frame) { return on_new_frame(frame); },
        [&]() { on_quit(); }),
      state{gui_state{*this}},
      inspector{create_inspector(
        *resources.renderer.get(),
        resources.text_material,
        resources.font.get(),
        lm::size2i{
          resources.window_size.width / 5,
          resources.window_size.height,
        })},
      map_editor{map_editor_init{
        .map = map,
        .renderer = resources.renderer.get(),
        .position = {inspector->get_size().width, 0},
        .size =
          lm::size2i{
            resources.window_size.width - 2 * inspector->get_size().width,
            resources.window_size.height,
          },
        .selection_outline_colour = std::array{0.7f, 0.5f, 0.5f},
        .text_material = resources.text_material,
        .font = resources.font.get(),
      }()},
      entity_list{entity_list_init{
        .renderer = *resources.renderer,
        .text_material = resources.text_material,
        .rect_material = resources.rect_material,
        .font = resources.font.get(),
        .position =
          lm::point2i{
            inspector->get_size().width + map_editor->get_size().width,
            0,
          },
        .size = inspector->get_size(),
      }()},
      active_panel_border{std::make_unique<lmtk::rect_border>(lmtk::rect_border{
        resources.renderer.get(),
        resources.border_material,
        {0, 0},
        {0, 0},
        resources.active_panel_border_colour,
        1.f,
      })},
      panel_order_horizontal{
        inspector.get(),
        map_editor.get(),
        entity_list.get(),
      },
      visible_panels{
        map_editor.get(),
        inspector.get(),
        entity_list.get(),
      },
      input_handlers{
        {map_editor.get(),
         [](auto &app, auto &ev) { return app.map_editor_handle(ev); }},
        {inspector.get(),
         [](auto &app, auto &ev) { return app.inspector_handle(ev); }},
        {entity_list.get(),
         [](auto &app, auto &ev) { return app.entity_list_handle(ev); }},
      }
{
    focus_tool_panel(visible_panels.front());
}

bool editor_app::on_new_frame(lmgl::iframe *frame)
{
    state >> lm::variant_visitor{
               [&](auto &state) { state.add_to_frame(*this, frame); },
             };

    return state >> lm::variant_visitor{
                      [&](player_state &player_state) { return true; },
                      [&](auto &) { return false; },
                    };
}

bool editor_app::on_input_event(lmtk::input_event const &input_event)
{
    return state >> lm::variant_visitor{
                      [&](auto &state_alternative) {
                          return state_alternative.handle_input_event(
                            {input_event, resources.resource_sink, *this});
                      },
                    };
}

void editor_app::on_quit()
{
    state >> lm::variant_visitor{
               [&](auto &state_alternative) {
                   state_alternative.move_resources(*this);
               },
             };
    active_panel_border->move_resources(
      resources.renderer.get(), resources.resource_sink);
    entity_list->move_resources(
      resources.renderer.get(), resources.resource_sink);
    map_editor->move_resources(
      resources.renderer.get(), resources.resource_sink);
    inspector->move_resources(
      resources.renderer.get(), resources.resource_sink);
    resources.free();
}

void editor_app::focus_tool_panel(itool_panel *tool_panel)
{
    active_panel_border->set_rect(
      tool_panel->get_position(), tool_panel->get_size());
}

void editor_app::toggle_tool_panel(itool_panel *tool_panel)
{
    auto found_visible = ranges::find(visible_panels, tool_panel);

    bool was_focused = found_visible == visible_panels.begin();
    bool was_visible = found_visible != visible_panels.end();

    if (!was_visible)
    {
        visible_panels.insert(visible_panels.begin(), tool_panel);
        refit_visible_panels();
        focus_tool_panel(visible_panels.front());
        return;
    }

    visible_panels.erase(found_visible);

    if (was_focused && tool_panel != map_editor.get())
    {
        refit_visible_panels();
    }
    else
    {
        visible_panels.insert(visible_panels.begin(), tool_panel);
    }
    focus_tool_panel(visible_panels.front());
}

void editor_app::refit_visible_panels()
{
    int total_width{0};

    for (auto panel : visible_panels)
    {
        if (panel == map_editor.get())
            continue;

        total_width += panel->get_size().width;
    }

    int remainder = resources.window_size.width - total_width;

    auto map_editor_size = map_editor->get_size();
    map_editor_size.width = remainder;
    map_editor->set_rect({0, 0}, map_editor_size);

    int current_pos{0};
    for (auto panel : panel_order_horizontal)
    {
        if (ranges::find(visible_panels, panel) == visible_panels.end())
            continue;

        auto panel_size = panel->get_size();

        panel->set_rect({current_pos, 0}, panel_size);
        current_pos += panel_size.width;
    }
}

map_selector editor_app::create_map_selector()
{
    return map_selector(map_selector_init{
      .directory = project_dir,
      .renderer = resources.renderer.get(),
      .font_material = resources.text_material,
      .font = resources.font.get(),
      .rect_material = resources.rect_material,
    });
}

modal_state editor_app::create_simulation_select_state()
{
    return modal_state{
      .modal = std::make_unique<lmtk::text_line_selector>(
        lmtk::text_line_selector_init{.lines = resources.simulation_names,
                                      .renderer = resources.renderer.get(),
                                      .font_material = resources.text_material,
                                      .font = resources.font.get(),
                                      .rect_material =
                                        resources.rect_material}()),
      .input_handler =
        [](auto &app, auto widget_ptr, auto &input_event) {
            auto selector =
              dynamic_cast<lmtk::text_line_selector *>(widget_ptr);
            return input_event >>
                   lm::variant_visitor{
                     [&](lmtk::key_down_event const &key_down_event) {
                         if (key_down_event.key == lmpl::key_code::Enter)
                         {
                             app.resources.selected_simulation_index =
                               selector->get_selection_index();
                             selector->move_resources(
                               app.resources.renderer.get(),
                               app.resources.resource_sink);
                             app.state = gui_state{app};
                             return true;
                         }
                         return selector->handle(key_down_event);
                     },
                     [&](auto &event_alternative) {
                         return selector->handle(event_alternative);
                     }};
        },
      .renderer =
        [](auto widget, auto frame) {
            dynamic_cast<lmtk::text_line_selector *>(widget)->add_to_frame(
              frame);
        },
    };
}

void editor_app::sync_entity_list()
{
    entity_list->display(
      *resources.renderer, resources.resource_sink, *resources.font, map);
}

editor_app::~editor_app() {}

void editor_app::init_map_selector()
{
    state.emplace<modal_state>(modal_state{
      .modal = std::make_unique<map_selector>(create_map_selector()),
      .input_handler =
        [](editor_app &app, auto widget, auto &input_event) {
            return dynamic_cast<map_selector *>(widget)->handle(
              input_event,
              map_selector_event_handler{[&](map_selector_chose_map const &ev) {
                  app.load_map(ev.path_to_file);
                  app.state.emplace<gui_state>(app);
              }});
        },
      .renderer =
        [](auto widget, auto frame) {
            dynamic_cast<map_selector *>(widget)->add_to_frame(frame);
        },
    });
}

void editor_app::init_command_help()
{
    state = modal_state{
      .modal = std::make_unique<command_help>(command_help_init{
        .renderer = *resources.renderer,
        .material = resources.text_material,
        .font = resources.font.get(),
        .commands = visible_panels.front()->get_command_descriptions(),
      }),
      .input_handler =
        [](editor_app &app, auto help_widget, auto &input_event) {
            return dynamic_cast<command_help *>(help_widget)
              ->handle(
                input_event,
                app.resources.renderer.get(),
                app.resources.font.get(),
                app.resources.resource_sink);
        },
      .renderer =
        [](auto help_widget, auto frame) {
            dynamic_cast<command_help *>(help_widget)->add_to_frame(frame);
        },
    };
}

void editor_app::init_map_saver()
{
    auto path = map_file_project_relative_path.string();
    state.emplace<modal_state>(modal_state{
      .modal = std::make_unique<saver>(
        resources, "Save map", path.substr(0, path.rfind(".lmap"))),
      .input_handler =
        [](editor_app &app, auto widget, auto &input_event) {
            auto saver = dynamic_cast<lmeditor::saver *>(widget);
            auto maybe_key_down_msg =
              std::get_if<lmtk::key_down_event>(&input_event);
            if (
              maybe_key_down_msg &&
              maybe_key_down_msg->key == lmpl::key_code::Enter)
            {
                auto relative = saver->field.get_value();
                auto absolute = app.project_dir / (relative + ".lmap");

                app.save_map(absolute);

                app.map_file_project_relative_path = relative;
                app.state.emplace<gui_state>(app);
                return true;
            }

            return saver->field.handle(
              input_event,
              app.resources.renderer.get(),
              app.resources.font.get(),
              app.resources.resource_sink);
        },
      .renderer =
        [](auto saver, auto frame) {
            dynamic_cast<lmeditor::saver *>(saver)->add_to_frame(frame);
        },
    });
}

void editor_app::init_pose_saver()
{
    state.emplace<modal_state>(modal_state{
      .modal = std::make_unique<saver>(
        resources,
        "Save pose",
        lmng::get_name(map, map_editor->get_selection())),
      .input_handler =
        [](editor_app &app, auto widget, auto &input_event) {
            auto saver = dynamic_cast<lmeditor::saver *>(widget);
            auto maybe_key_down_msg =
              std::get_if<lmtk::key_down_event>(&input_event);
            if (
              maybe_key_down_msg &&
              maybe_key_down_msg->key == lmpl::key_code::Enter)
            {
                auto relative = saver->field.get_value();
                auto absolute = app.project_dir / (relative + ".lpose");

                std::ofstream output{absolute};
                output << lmeditor::save_pose(
                  app.map, app.map_editor->get_selection());

                app.state.emplace<gui_state>(app);
                return true;
            }

            return saver->field.handle(
              input_event,
              app.resources.renderer.get(),
              app.resources.font.get(),
              app.resources.resource_sink);
        },
      .renderer =
        [](auto saver, auto frame) {
            dynamic_cast<lmeditor::saver *>(saver)->add_to_frame(frame);
        },
    });
}

void editor_app::init_pose_loader()
{
    auto is_lpose_file = [](auto &file) {
        return file.path().extension() == ".lpose";
    };

    auto proj_relative_no_ext = [&](auto &entry) {
        auto with_ext = std::filesystem::relative(entry, project_dir).string();
        return with_ext.substr(
          0, with_ext.size() - std::string{".lpose"}.size());
    };

    auto pose_paths =
      std::filesystem::recursive_directory_iterator{project_dir} |
      ranges::views::filter(is_lpose_file) |
      ranges::views::transform(proj_relative_no_ext) |
      ranges::to<std::vector<std::string>>();

    state.emplace<modal_state>(modal_state{
      .modal = std::make_unique<lmtk::text_line_selector>(
        lmtk::text_line_selector_init{.lines = pose_paths,
                                      .renderer = resources.renderer.get(),
                                      .font_material = resources.text_material,
                                      .font = resources.font.get(),
                                      .rect_material =
                                        resources.rect_material}()),
      .input_handler =
        [pose_paths = std::move(pose_paths)](
          editor_app &app, auto widget_ptr, auto &input_event) {
            auto selector =
              dynamic_cast<lmtk::text_line_selector *>(widget_ptr);

            return input_event >>
                   lm::variant_visitor{
                     [&](lmtk::key_down_event const &key_down_event) {
                         if (key_down_event.key == lmpl::key_code::Enter)
                         {
                             auto pose_path =
                               app.project_dir /
                               (pose_paths[selector->get_selection_index()] +
                                ".lpose");

                             load_pose(
                               app.map,
                               app.map_editor->get_selection(),
                               YAML::LoadFile(pose_path));

                             app.state.emplace<gui_state>(app);
                             return true;
                         }
                         return selector->handle(key_down_event);
                     },
                     [&](auto &event_alternative) {
                         return selector->handle(event_alternative);
                     }};
        },
      .renderer =
        [](auto widget, auto frame) {
            dynamic_cast<lmtk::text_line_selector *>(widget)->add_to_frame(
              frame);
        },
    });
}
} // namespace lmeditor
