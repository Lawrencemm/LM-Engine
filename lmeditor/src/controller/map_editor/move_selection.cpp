#include "map_editor_controller.h"

#include <range/v3/algorithm/min_element.hpp>

#include <lmlib/eigen.h>
#include <lmng/transform.h>

namespace lmeditor
{
entt::entity map_editor_controller::nearest_entity(
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

    for (auto other_entity : transforms)
    {
        if (other_entity == entity)
            continue;

        Eigen::Vector3f to_other =
          lmng::resolve_transform(map, other_entity).position -
          transform.position;

        float to_dot_dir = to_other.dot(direction);

        if (to_dot_dir > 0.01f && to_dot_dir < nearest_dot_dir)
        {
            nearest_entity = other_entity;
            nearest_dot_dir = to_dot_dir;
        }
    }

    return nearest_entity;
}

entt::entity map_editor_controller::farthest_entity(
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

    for (auto curr : transforms)
    {
        auto transform = lmng::resolve_transform(map, curr);
        auto direction_dot_position = transform.position.dot(direction);

        if (direction_dot_position > max_dot)
        {
            max_dot = direction_dot_position;
            farthest = curr;
        }
    }

    return farthest;
}

bool map_editor_controller::move_selection(
  entt::registry const &map,
  Eigen::Vector3f const &direction)
{
    if (!have_selection())
    {
        auto farthest = farthest_entity(map, direction);
        if (!map.valid(farthest))
            return false;

        select(farthest);
        return true;
    }

    auto nearest = nearest_entity(map, get_selection(), direction);

    if (!map.valid(nearest))
        return false;

    select(nearest);
    return true;
}

bool map_editor_controller::move_selection_view(
  entt::registry const &map,
  const Eigen::Vector3f &view_axis)
{
    return move_selection(map, lm::snap_to_axis(camera.rotation * view_axis));
}

void map_editor_controller::set_map(const entt::registry &registry)
{
    state.emplace<select_state>(*this);
    clear_selection();
}
} // namespace lmeditor
