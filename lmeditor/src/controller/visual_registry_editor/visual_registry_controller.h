#pragma once

#include "../../controller/viewport/viewport.h"
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <lmeditor/model/command.h>
#include <lmlib/variant_visitor.h>
#include <lmng/hierarchy.h>
#include <lmng/physics.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>
#include <lmtk/input_event.h>
#include <variant>

namespace lmeditor
{
class visual_registry_controller : public viewport
{
  public:
    explicit visual_registry_controller(
      entt::registry &map,
      orbital_camera_init const &camera_init);

    bool handle(lmtk::input_event const &event);

    entt::entity add_cube(
      entt::registry &map,
      Eigen::Vector3f const &position,
      float extent);

    entt::registry *map;
    lmng::hierarchy_system hierarchy_system;

    bool have_selection() const;

    void translate(
      entt::registry &map,
      entt::entity entity,
      Eigen::Vector3f const &vector);

    Eigen::Vector3f view_to_world(Eigen::Vector3f const &view_vector);

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
      Eigen::Vector3f const &direction);

    bool move_selection_view(
      entt::registry const &map,
      Eigen::Vector3f const &view_axis);

    entt::entity
      add_adjacent(entt::registry &map, Eigen::Vector3f const &direction);

    entt::entity
      copy_entity(entt::registry &map, Eigen::Vector3f const &direction);

    std::string get_unique_name(entt::registry const &map, char const *prefix);

    std::vector<command_description> get_command_descriptions();

    entt::entity get_selection() const;

    Eigen::Vector3f get_selection_position(entt::registry const &map) const;

    Eigen::Vector3f get_selection_extents(entt::registry const &map) const;

    struct command_args
    {
        class visual_registry_controller &controller;
        entt::registry &map;
        lmtk::key_down_event const &key_down_event;
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
        explicit select_state(other_state_type &,
          visual_registry_controller &model)
            : select_state{model} {};

        explicit select_state(visual_registry_controller &model);

        command_list commands;
        command_map key_command_map;
    };
    struct translate_state
    {
        using parent_state_type = select_state;
        constexpr static auto label = "Translate Entity";

        explicit translate_state(select_state &, visual_registry_controller &);

        command_list commands;
        command_map key_command_map;
    };
    struct add_adjacent_state
    {
        constexpr static auto label = "Add Entity Adjacent";
        using parent_state_type = select_state;

        explicit add_adjacent_state(
          visual_registry_controller::select_state &,
          visual_registry_controller &model);

        command_list commands;
        command_map key_command_map;
    };
    struct copy_entity_state
    {
        constexpr static auto label = "Copy Entity Adjacent";
        using parent_state_type = select_state;

        copy_entity_state(
          visual_registry_controller::select_state &,
          visual_registry_controller &model);

        command_list commands;
        command_map key_command_map;
    };
    struct reparent_state
    {
        constexpr static auto label = "Reparent Entity";
        using parent_state_type = select_state;

        reparent_state(select_state &, visual_registry_controller &model);

        command_list commands;
        command_map key_command_map;
        entt::entity entity;
    };
    struct scale_state
    {
        constexpr static auto label = "Scale Entity";
        using parent_state_type = select_state;

        scale_state(select_state &, visual_registry_controller &model);

        command_list commands;
        command_map key_command_map;
    };
    struct rotate_state
    {
        constexpr static auto label = "Rotate Entity";
        using parent_state_type = select_state;

        rotate_state(select_state &, visual_registry_controller &model);

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
    auto enter_state(current_state_type &current_state);

    void leave_state()
    {
        state >>
          lm::variant_visitor{
            [&](auto &current_state) {
                using state_type =
                  typename std::remove_reference<decltype(current_state)>::type;
                using new_state_type = typename state_type::parent_state_type;
                state_text = new_state_type::label;
                state = new_state_type{current_state, *this};
            },
            [&](select_state &current_state) {
                throw std::runtime_error{"Tried to leave map editor controller "
                                         "select state: no parent state"};
            },
          };
    }

    void set_map(entt::registry const &registry);

    void select(entt::entity entity);
    entt::registry *get_registry() const;
    lm::camera get_camera() const;
    void clear_selection() const;
};

template <typename new_state_type, typename current_state_type>
auto visual_registry_controller::enter_state(current_state_type &current_state)
{
    static_assert(
      std::is_same_v<
        current_state_type,
        typename new_state_type::parent_state_type>,
      "New state must have current state as parent.");

    state = new_state_type{current_state, *this};
    state_text = new_state_type::label;
    return true;
}
} // namespace lmeditor
