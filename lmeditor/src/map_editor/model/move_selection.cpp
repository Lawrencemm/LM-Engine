#include "model.h"

#include <range/v3/algorithm/min_element.hpp>

#include <lmengine/transform.h>
#include <lmlib/eigen.h>

namespace lmeditor
{
entt::entity map_editor_model::nearest_entity(
  entt::registry const &map,
  entt::entity entity,
  const Eigen::Vector3f &direction)
{
    auto transforms = map.view<lmng::transform const>();

    if (transforms.size() == 1)
    {
        return entt::null;
    }

    entt::entity nearest_entity{entt::null};
    float nearest_dot_dir{std::numeric_limits<float>::infinity()};

    auto transform = lmng::resolve_transform(map, entity);

    transforms.each([&](auto other_entity, auto &) {
        if (other_entity == entity)
            return;

        Eigen::Vector3f to_other =
          lmng::resolve_transform(map, other_entity).position -
          transform.position;

        float to_dot_dir = to_other.dot(direction);

        if (to_dot_dir > 0.01f && to_dot_dir < nearest_dot_dir)
        {
            nearest_entity = other_entity;
            nearest_dot_dir = to_dot_dir;
        }
    });

    return nearest_entity;
}

entt::entity map_editor_model::farthest_entity(
  entt::registry const &map,
  const Eigen::Vector3f &direction)
{
    auto transforms = map.view<lmng::transform const>();

    if (transforms.empty())
        return entt::null;
    else if (transforms.size() == 1)
        return *transforms.begin();

    float max_dot{-std::numeric_limits<float>::infinity()};
    entt::entity farthest{entt::null};

    transforms.each([&](auto curr, auto &) {
        auto transform = lmng::resolve_transform(map, curr);
        auto direction_dot_position = transform.position.dot(direction);

        if (direction_dot_position > max_dot)
        {
            max_dot = direction_dot_position;
            farthest = curr;
        }
    });

    return farthest;
}

bool map_editor_model::move_selection(
  entt::registry const &map,
  Eigen::Vector3f const &direction,
  map_editor_event_handler const &event_handler)
{
    if (!have_selection())
    {
        auto farthest = farthest_entity(map, direction);
        if (!map.valid(farthest))
            return false;

        select_box(farthest);
        event_handler(map_editor_changed_selection{map, selected_box});
        return true;
    }

    auto nearest = nearest_entity(map, selected_box, direction);

    if (!map.valid(nearest))
        return false;

    select_box(nearest);
    event_handler(map_editor_changed_selection{map, selected_box});
    return true;
}

bool map_editor_model::move_selection_view(
  entt::registry const &map,
  const Eigen::Vector3f &view_axis,
  map_editor_event_handler const &event_handler)
{
    return move_selection(
      map, lm::snap_to_axis(camera.rotation * view_axis), event_handler);
}

void map_editor_model::set_map(const entt::registry &registry)
{
    state.emplace<select_state>(*this);
    selected_box = entt::null;
}
} // namespace lmeditor
