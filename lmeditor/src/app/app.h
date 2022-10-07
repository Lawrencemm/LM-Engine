#pragma once

#include <boost/dll/shared_library.hpp>
#include <filesystem>
#include <lmeditor/component/inspector.h>
#include <lmeditor/component/map_editor.h>
#include <lmeditor/model/trimesh.h>
#include <lmeditor/project_plugin.h>
#include <lmeditor/simulation_plugin.h>
#include <lmhuv.h>
#include <lmlib/application.h>
#include <lmlib/realtime_clock.h>
#include <lmlib/variant_visitor.h>
#include <lmng/meta/signal.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/component.h>
#include <lmtk/rect_border.h>
#include <lmtk/text_layout.h>
#include <set>
#include <tbb/flow_graph.h>

namespace lmeditor
{
class editor_app : public lmng::any_component_listener
{
  public:
    explicit editor_app(const std::filesystem::path &project_dir);
    editor_app(editor_app const &) = delete;
    editor_app(editor_app &&) = delete;
    void main();
    ~editor_app() = default;

    void on_construct_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) override;
    void on_update_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) override;
    void on_destroy_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) override;

  private:
    struct gui_state
    {
        explicit gui_state(editor_app &app);
        bool handle(editor_app &app, lmtk::input_event const &input_event);
        bool add_to_frame(editor_app &app, lmgl::iframe *frame);
        void move_resources(
          lmgl::irenderer *renderer,
          lmgl::resource_sink &resource_sink);
    };
    struct modal_state
    {
        lmtk::component modal;

        bool handle(editor_app &app, lmtk::input_event const &input_event);
        bool add_to_frame(editor_app &app, lmgl::iframe *frame);
        void move_resources(
          lmgl::irenderer *renderer,
          lmgl::resource_sink &resource_sink);

        std::function<void(lmtk::widget_interface *, lmgl::iframe *)> renderer;
    };

    using state_variant_type = std::variant<gui_state, modal_state>;

    std::filesystem::path project_dir;

    lmtk::app_resources resources;
    lmtk::resource_cache resource_cache;
    lmng::asset_cache asset_cache;
    lmtk::app_flow_graph flow_graph;

    struct save_map_msg
    {
        std::shared_ptr<entt::registry> map;
    };

    lm::overwrite_node<save_map_msg> save_map_buffer_node;
    tbb::flow::function_node<save_map_msg> save_map_node;

    lmeditor::project_plugin project_plugin;

    size_t selected_simulation_index{0};

    entt::registry map;

    std::size_t next_model_id{0}, next_component_id{0};
    std::map<std::size_t, void*> models;
    std::map<std::size_t, lmtk::component> components;

    std::map<std::size_t, std::size_t> component_model_map;

    std::unique_ptr<lmtk::rect_border> active_component_border;
    std::vector<size_t> visible_components;
    std::vector<size_t> component_order;
    size_t main_component_id;

    std::map<lmpl::key_code, size_t> key_code_view_map;

    std::string map_file_project_relative_path{};

    state_variant_type state;

    lmtk::component create_simulation_selector();
    lmtk::component create_map_saver();
    lmtk::component create_command_help();
    lmtk::component create_pose_loader();
    lmtk::component create_pose_saver(std::string initial_project_path);
    lmtk::component create_player();

    lmtk::component &get_current_component()
    {
        return components[visible_components.front()];
    }

    bool on_map_selected(unsigned _unused_, const std::string &project_path);
    bool on_simulation_selected(unsigned selection_index);
    bool on_pose_selected(
      unsigned selection_index,
      std::string const &project_path);
    bool on_map_saved(const std::string &project_path);
    bool on_pose_saved(const std::string &project_path);

    bool load_map(std::filesystem::path const &path);
    void save_map(
      std::filesystem::path const &absolute_path,
      entt::registry &map_registry);
    void save_map_async();

  protected:
    bool on_input_event(lmtk::input_event const &variant);
    bool on_new_frame(lmgl::iframe *frame);
    void on_quit();

    void refit_visible_components();
    void assign_view_key(lmpl::key_code code, size_t component_id);
    void toggle_component(size_t component_id);
    void update_active_component_border(lmtk::component_interface *tool_panel);

    void move_current_state_resources();

    template <typename new_state_type> void change_state()
    {
        move_current_state_resources();

        state.emplace<new_state_type>(*this);
    }

    template <typename new_state_type>
    void change_state(new_state_type new_state)
    {
        move_current_state_resources();

        state.emplace<new_state_type>(std::move(new_state));
    }
    std::any get_component_model(size_t i);
};
} // namespace lmeditor
