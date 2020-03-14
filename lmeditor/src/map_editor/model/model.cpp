#include "model.h"

#include <range/v3/algorithm/find_if.hpp>

#include <lmengine/name.h>
#include <lmengine/physics.h>
#include <lmengine/reflection.h>
#include <lmlib/eigen.h>
#include <range/v3/view/join.hpp>

namespace lmeditor
{
map_editor_model::map_editor_model(init const &init)
    : state{select_state{*this}},
      state_text{select_state::label},
      viewport{{init.camera_init}},
      selected_box{entt::null}
{
}

bool map_editor_model::handle(
  lmtk::input_event const &event,
  map_editor_event_handler const &event_handler,
  state_change_handler on_state_change)
{
    return std::visit(
      lm::variant_visitor{
        [&](
          lmtk::key_down_event const &key_down_event, auto &state_alternative) {
            auto itfound = state_alternative.key_command_map.find(
              key_down_event.input_state.key_state);

            if (itfound == state_alternative.key_command_map.end())
            {
                itfound =
                  state_alternative.key_command_map.find(key_down_event.key);

                if (itfound == state_alternative.key_command_map.end())
                    return false;
            }

            return itfound->second.fn(command_args{
              &state_alternative,
              *this,
              event_handler,
              key_down_event,
              on_state_change,
            });
        },
        [](auto &, auto &) { return false; },
      },
      event,
      state);
}

void map_editor_model::add_component_to_selected(entt::meta_type const &type)
{
    auto component = type.ctor().invoke();
    lmng::assign_to_entity(component, map, selected_box);
}

bool map_editor_model::update_selection(
  entt::meta_data const &data,
  std::string const &string_repr)
{
    auto component = lmng::any_component{map, selected_box, data.parent()};
    component.set(data, string_repr, map);
    component.replace(map, selected_box);
    return true;
}

entt::entity
  map_editor_model::add_cube(Eigen::Vector3f const &position, float extent)
{
    auto new_cube = map.create();

    auto &transform = map.assign<lmng::transform>(
      new_cube, position, Eigen::Quaternionf::Identity());

    auto &box = map.assign<lmng::box_render>(
      new_cube,
      Eigen::Vector3f{extent, extent, extent},
      std::array{0.5f, 0.5f, 0.5f});

    map.assign<lmng::box_collider>(
      new_cube, Eigen::Vector3f{extent, extent, extent});

    auto &rigid_body =
      map.assign<lmng::rigid_body>(new_cube, 1.f, 0.25f, 0.75f);

    map.assign<lmng::name>(new_cube, get_unique_name("Box"));

    return new_cube;
}

std::string map_editor_model::get_unique_name(char const *prefix)
{
    std::string unique_name{prefix};

    unsigned postfix{0};

    auto name_view = map.view<lmng::name>();

    while (auto found_existing = ranges::find_if(name_view, [&](auto entity) {
                                     return name_view.get(entity).string ==
                                            unique_name;
                                 }) != name_view.end())
    {
        unique_name = std::string{prefix} + std::to_string(++postfix);
    }

    return unique_name;
}

entt::entity map_editor_model::add_adjacent(Eigen::Vector3f const &direction)
{
    auto add_pos = get_selection_position();
    auto extents = get_selection_extents();
    extents += Eigen::Vector3f::Ones();
    add_pos += std::abs(extents.dot(direction)) * direction;
    return add_cube(add_pos, 1.f);
}

entt::entity map_editor_model::copy_entity(Eigen::Vector3f const &direction)
{
    auto new_name = get_unique_name(lmng::get_name(map, selected_box).c_str());
    auto new_box = map.create(selected_box);
    lmng::reflect_components(
      map, selected_box, [&](lmng::any_component component) {
          component.assign(map, new_box);
      });
    auto &transform = map.get<lmng::transform>(new_box);
    transform.position +=
      2 * Eigen::Vector3f{direction.array() * get_selection_extents().array()};
    map.assign<lmng::name>(new_box, lmng::name{new_name});
    return new_box;
}

void map_editor_model::translate(
  entt::entity entity,
  Eigen::Vector3f const &vector)
{
    map.get<lmng::transform>(entity).position += vector;
}

std::vector<command_description> map_editor_model::get_command_descriptions()
{
    return state >> lm::variant_visitor{[this](auto &state_alternative) {
               return get_command_descriptions(
                 state_alternative.commands,
                 std::string{state_alternative.label}.empty()
                   ? "Map Editor"
                   : std::string{"Map Editor -> "} + state_alternative.label);
           }};
}

std::vector<command_description> map_editor_model::get_command_descriptions(
  map_editor_model::command_list const &list,
  std::string const &context)
{
    std::vector<command_description> command_lines;
    for (auto &[keys, command] : list)
    {
        std::vector<std::string> key_names;
        for (auto key : keys)
            key_names.emplace_back(lmpl::key_code_name_map.at(key));

        std::string keystroke_string;
        command_lines.emplace_back(command_description{
          command.description,
          ranges::view::join(key_names, ' '),
          context,
        });
    }
    return std::move(command_lines);
}

Eigen::Vector3f
  map_editor_model::view_to_axis(Eigen::Vector3f const &view_vector)
{
    return lm::snap_to_axis(view_to_world(view_vector));
}

Eigen::Vector3f map_editor_model::get_selection_extents() const
{

    auto maybe_box_render = map.try_get<lmng::box_render>(selected_box);
    if (maybe_box_render)
        return maybe_box_render->extents;

    auto maybe_box_collider = map.try_get<lmng::box_collider>(selected_box);
    if (maybe_box_collider)
        return maybe_box_collider->extents;

    return Eigen::Vector3f{1.f, 1.f, 1.f};
}
} // namespace lmeditor
