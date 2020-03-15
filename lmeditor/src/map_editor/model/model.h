#pragma once

#include "../../viewport/viewport.h"
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <lmeditor/map_editor.h>
#include <lmeditor/map_editor_events.h>
#include <lmengine/physics.h>
#include <lmengine/shapes.h>
#include <lmengine/transform.h>
#include <variant>

namespace lmeditor
{
class map_editor_model : public viewport
{
  public:
    struct init;

    explicit map_editor_model(init const &init);

    using state_change_handler = std::function<void(map_editor_model const &)>;

    bool handle(
      entt::registry &map,
      lmtk::input_event const &event,
      const map_editor_event_handler &event_handler = {},
      state_change_handler on_state_change = [](auto &) {});

    entt::entity add_cube(
      entt::registry &map,
      Eigen::Vector3f const &position,
      float extent);

    entt::entity selected_box{entt::null};

    void select_box(entt::entity entity) { selected_box = entity; }

    bool have_selection() { return selected_box != entt::null; }

    void translate(
      entt::registry &map,
      entt::entity entity,
      Eigen::Vector3f const &vector);

    Eigen::Vector3f view_to_world(Eigen::Vector3f const &view_vector)
    {
        return camera.rotation * view_vector;
    }

    Eigen::Vector3f view_to_axis(Eigen::Vector3f const &view_vector);

    entt::entity nearest_entity(
      entt::registry const &map,
      entt::entity entity,
      Eigen::Vector3f const &direction);
    entt::entity farthest_entity(
      entt::registry const &map,
      Eigen::Vector3f const &direction);
    bool move_selection(
      entt::registry const &map,
      Eigen::Vector3f const &direction,
      map_editor_event_handler const &event_handler = {});
    bool move_selection_view(
      entt::registry const &map,
      Eigen::Vector3f const &view_axis,
      map_editor_event_handler const &event_handler);

    entt::entity
      add_adjacent(entt::registry &map, Eigen::Vector3f const &direction);
    entt::entity
      copy_entity(entt::registry &map, Eigen::Vector3f const &direction);

    std::string get_unique_name(entt::registry const &map, char const *prefix);

    std::vector<command_description> get_command_descriptions();

    Eigen::Vector3f get_selection_position(entt::registry const &map)
    {
        return lmng::resolve_transform(map, selected_box).position;
    }

    Eigen::Vector3f get_selection_extents(entt::registry const &map) const;

    struct command_args
    {
        void *state_ptr;
        class map_editor_model &model;
        entt::registry &map;
        map_editor_event_handler const &event_handler;
        lmtk::key_down_event const &key_down_event;
        state_change_handler const &on_state_change;
    };

    struct command
    {
        std::function<bool(command_args const &)> fn;
        std::string description;
    };

    using command_map = std::unordered_map<lmpl::key_state, command>;
    using command_list =
      std::vector<std::pair<std::vector<lmpl::key_code>, command>>;

    static command_list viewport_commands;
    static command_list move_selection_commands;

    static std::vector<command_description> get_command_descriptions(
      command_list const &list,
      std::string const &context);

    struct select_state
    {
        constexpr static auto label = "";

        template <typename other_state_type>
        explicit select_state(other_state_type &, map_editor_model &model)
            : select_state{model} {};

        explicit select_state(map_editor_model &model);

        command_list commands;
        command_map key_command_map;
    };
    struct translate_state
    {
        using parent_state_type = select_state;
        constexpr static auto label = "Translate Entity";

        explicit translate_state(select_state &, map_editor_model &);

        command_list commands;
        command_map key_command_map;
    };
    struct add_adjacent_state
    {
        constexpr static auto label = "Add Entity Adjacent";
        using parent_state_type = select_state;

        explicit add_adjacent_state(
          map_editor_model::select_state &,
          map_editor_model &model);

        command_list commands;
        command_map key_command_map;
    };
    struct copy_entity_state
    {
        constexpr static auto label = "Copy Entity Adjacent";
        using parent_state_type = select_state;

        copy_entity_state(
          map_editor_model::select_state &,
          map_editor_model &model);

        command_list commands;
        command_map key_command_map;
    };
    struct reparent_state
    {
        constexpr static auto label = "Reparent Entity";
        using parent_state_type = select_state;

        reparent_state(select_state &, map_editor_model &model);

        command_list commands;
        command_map key_command_map;
        entt::entity entity;
    };
    struct scale_state
    {
        constexpr static auto label = "Scale Entity";
        using parent_state_type = select_state;

        scale_state(select_state &, map_editor_model &model);

        command_list commands;
        command_map key_command_map;
    };
    struct rotate_state
    {
        constexpr static auto label = "Rotate Entity";
        using parent_state_type = select_state;

        rotate_state(select_state &, map_editor_model &model);

        command_list commands;
        command_map key_command_map;
    };

    using state_variant_type = std::variant<
      select_state,
      translate_state,
      add_adjacent_state,
      copy_entity_state,
      reparent_state,
      scale_state,
      rotate_state>;

    state_variant_type state;
    std::string state_text;

    template <typename new_state_type, typename current_state_type>
    auto enter_state(
      current_state_type &current_state,
      state_change_handler const &on_state_change);

    template <typename state_type>
    void leave_state(
      state_type &current_state,
      state_change_handler on_state_change);
    void set_map(entt::registry const &registry);
};

template <typename new_state_type, typename current_state_type>
auto map_editor_model::enter_state(
  current_state_type &current_state,
  state_change_handler const &on_state_change)
{
    static_assert(
      std::is_same_v<
        current_state_type,
        typename new_state_type::parent_state_type>,
      "New state must have current state as parent.");

    state = new_state_type{current_state, *this};
    state_text = new_state_type::label;
    on_state_change(*this);
    return true;
}

template <typename state_type>
void map_editor_model::leave_state(
  state_type &current_state,
  state_change_handler on_state_change)
{
    using new_state_type = typename state_type::parent_state_type;
    state_text = new_state_type::label;
    state = new_state_type{current_state, *this};
    on_state_change(*this);
}

struct map_editor_model::init
{
    orbital_camera_init camera_init;
};
} // namespace lmeditor
