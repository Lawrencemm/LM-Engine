#include <random>

#include <boost/rational.hpp>

#include <lmeditor/map_editor.h>
#include <lmlib/variant_visitor.h>
#include <lmtk/text_line_selector.h>
#include <range/v3/view.hpp>

#include "app.h"

using namespace tbb::flow;

namespace lmeditor
{
editor_app::editor_app(const std::filesystem::path &project_dir)
    : resources{project_dir},
      flow_graph(
        resources,
        [&](auto &ev) { return on_input_event(ev); },
        [&](auto frame) { return on_new_frame(frame); },
        [&]() { on_quit(); }),
      state{gui_state{*this}},
      panel_order_horizontal{
        resources.inspector.get(),
        resources.map_editor.get(),
        resources.entity_list.get(),
      },
      visible_panels{
        resources.map_editor.get(),
        resources.inspector.get(),
        resources.entity_list.get(),
      },
      input_handlers{
        {resources.map_editor.get(),
         [](auto &app, auto &ev) { return app.map_editor_handle(ev); }},
        {resources.inspector.get(),
         [](auto &app, auto &ev) { return app.inspector_handle(ev); }},
        {resources.entity_list.get(),
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
    resources.free();
}

void editor_app::focus_tool_panel(itool_panel *tool_panel)
{
    resources.active_panel_border->set_rect(
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

    if (was_focused && tool_panel != resources.map_editor.get())
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
        if (panel == resources.map_editor.get())
            continue;

        total_width += panel->get_size().width;
    }

    int remainder = resources.window_size.width - total_width;

    auto map_editor_size = resources.map_editor->get_size();
    map_editor_size.width = remainder;
    resources.map_editor->set_rect({0, 0}, map_editor_size);

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
      .directory = resources.project_dir,
      .renderer = resources.renderer.get(),
      .font_material = resources.text_material,
      .font = resources.font.get(),
      .rect_material = resources.rect_material,
    });
}

modal_state editor_app::create_simulation_select_state()
{
    return modal_state{
      std::make_unique<lmtk::text_line_selector>(lmtk::text_line_selector_init{
        .lines = resources.simulation_names,
        .renderer = resources.renderer.get(),
        .font_material = resources.text_material,
        .font = resources.font.get(),
        .rect_material = resources.rect_material}),
      [](auto &app, auto widget_ptr, auto &input_event) {
          auto selector = dynamic_cast<lmtk::text_line_selector *>(widget_ptr);
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
      }};
}

void editor_app::sync_entity_list()
{
    resources.entity_list->display(
      *resources.renderer,
      resources.resource_sink,
      *resources.font,
      resources.map_editor->get_map());
}

editor_app::~editor_app() {}
} // namespace lmeditor
