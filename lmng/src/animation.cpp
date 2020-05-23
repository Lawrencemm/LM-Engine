#include <Eigen/Eigen>
#include <fmt/format.h>
#include <fstream>
#include <lmng/animation.h>
#include <lmng/name.h>
#include <lmng/serialisation.h>
#include <lmng/transform.h>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/zip.hpp>
#include <yaml-cpp/yaml.h>

namespace lmng
{
pose animation_system::load_pose(
  entt::registry const &registry,
  std::string const &pose_name,
  YAML::Node const &yaml)
{
    auto new_pose = pose{next_pose_index++};

    pose_data data;

    for (auto const &yaml_entity : yaml)
    {
        data.targets.emplace_back(
          deserialise_component(
            registry, "Transform", yaml_entity.second["Transform"])
            .cast<transform>());
        data.bone_names.emplace_back(yaml_entity.first.as<std::string>());
    }
    poses[new_pose] = data;
    pose_name_to_id.emplace(std::pair{pose_name, new_pose});

    return new_pose;
}

animation animation_system::load_animation(YAML::Node const &yaml)
{
    auto new_animation = animation{next_animation_index++};

    animation_data data;

    for (auto const &keyframe_yaml : yaml["Keyframes"])
    {
        data.keyframes.emplace_back(keyframe{
          pose_name_to_id.at(keyframe_yaml["Pose"].as<std::string>()),
          keyframe_yaml["Time"].as<float>(),
        });
    }

    animations.emplace(std::pair{new_animation, std::move(data)});

    return new_animation;
}

void animation_system::update(entt::registry &registry, float dt)
{
    registry.view<animation_state>().each(
      [&](auto entity, animation_state &animation_state) {
          update_animation(registry, entity, animation_state, dt);
      });
}

void animation_system::animate(
  entt::registry &registry,
  entt::entity skeleton_root,
  animation animation,
  float progress,
  float rate,
  anim_loop_type loop_type)
{
    registry.assign<animation_state>(
      skeleton_root, animation, rate, progress, loop_type);
}

void animation_system::update_animation(
  entt::registry &registry,
  entt::entity entity,
  animation_state &animation_state,
  float dt)
{
    auto &animation_data = animations[animation_state.animation];

    if (animation_state.rate == 0)
        return;

    auto [prev_keyframe, next_keyframe] =
      get_keyframes(animation_state, animation_data);

    float delta = animation_state.rate * dt;

    float keyframe_gap = std::abs(next_keyframe->time - prev_keyframe->time);

    float keyframe_progress =
      std::abs(animation_state.progress - prev_keyframe->time);

    tween_poses(
      registry,
      entity,
      poses[prev_keyframe->pose],
      poses[next_keyframe->pose],
      (keyframe_progress + delta) / keyframe_gap);

    advance_animation(animation_state, animation_data, next_keyframe, delta);
}

void animation_system::advance_animation(
  animation_state &animation_state,
  animation_system::animation_data const &animation_data,
  keyframe_list::iterator const &next_keyframe,
  float delta)
{
    float to_next = std::abs(next_keyframe->time - animation_state.progress);

    if (to_next - std::abs(delta) <= 0)
    {
        bool forward = !std::signbit(delta);

        bool at_end = forward
                        ? next_keyframe + 1 == animation_data.keyframes.end()
                        : next_keyframe == animation_data.keyframes.begin();

        if (at_end)
        {
            switch (animation_state.loop_type)
            {
            case anim_loop_type::pendulum:
                animation_state.rate = -animation_state.rate;
                delta = -delta;
            }
        }
    }

    animation_state.progress += delta;
}

auto animation_system::get_keyframes(
  animation_state &animation_state,
  animation_system::animation_data &animation_data)
  -> std::pair<keyframe_list::iterator, keyframe_list::iterator>
{
    keyframe_list::iterator prev;
    keyframe_list::iterator next;

    for (auto keyframe_iterator = animation_data.keyframes.begin();
         keyframe_iterator != animation_data.keyframes.end();
         ++keyframe_iterator)
    {
        if (keyframe_iterator->time < animation_state.progress)
        {
            if (animation_state.rate > 0)
            {
                prev = keyframe_iterator;
                next = keyframe_iterator + 1;
            }
            else
            {
                prev = keyframe_iterator + 1;
                next = keyframe_iterator;
            }
        }
    }

    return {prev, next};
}

void animation_system::tween_poses(
  entt::registry &registry,
  entt::entity entity,
  const animation_system::pose_data &first,
  const animation_system::pose_data &second,
  float distance)
{
    for (auto const &[bone_name, from, target] :
         ranges::views::zip(second.bone_names, first.targets, second.targets))
    {
        auto &transform =
          registry.get<lmng::transform>(lmng::find_entity(registry, bone_name));

        transform.position =
          from.position + distance * (target.position - from.position);

        transform.rotation = from.rotation.slerp(distance, target.rotation);
    }
}
} // namespace lmng
