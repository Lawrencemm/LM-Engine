#include <Eigen/Eigen>
#include <fmt/format.h>
#include <fstream>
#include <lmng/animation.h>
#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmng/transform.h>
#include <lmng/yaml_save_load.h>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/zip.hpp>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

namespace lmng
{
void animation_system::update(entt::registry &registry, float dt)
{
    for (auto [entity, animation_state] :
         registry.view<animation_state>().proxy())
    {
        update_animation(registry, entity, animation_state, dt);
    }
}

void animation_system::animate(
  entt::registry &registry,
  entt::entity skeleton_root,
  animation animation,
  float progress,
  float rate,
  anim_loop_type loop_type)
{
    registry.emplace<animation_state>(
      skeleton_root, animation, rate, progress, loop_type);
}

void animation_system::update_animation(
  entt::registry &registry,
  entt::entity entity,
  animation_state &animation_state,
  float dt)
{
    SPDLOG_INFO("Updating animation for {}", lmng::get_name(registry, entity));
    auto &animation_data = *animation_state.animation;

    if (animation_state.rate == 0)
        return;

    auto [prev_keyframe, next_keyframe] =
      get_keyframes(animation_state, animation_data);

    float delta = animation_state.rate * dt;

    float transition_period = next_keyframe->time - prev_keyframe->time;

    float transition_progress = animation_state.progress - prev_keyframe->time;

    tween_poses(
      registry,
      entity,
      *prev_keyframe->pose,
      *next_keyframe->pose,
      (transition_progress + delta) / transition_period);

    registry.replace<lmng::animation_state>(
      entity,
      advance_animation(
        animation_state, animation_data, prev_keyframe, next_keyframe, delta));
    SPDLOG_INFO("Finished updating animation for {}", lmng::get_name(registry, entity));
}

animation_state animation_system::advance_animation(
  animation_state const animation_state,
  animation_data const &animation_data,
  keyframe_list::iterator const &prev_keyframe,
  keyframe_list::iterator const &next_keyframe,
  float delta)
{
    lmng::animation_state new_state = animation_state;
    new_state.progress += delta;

    bool forward = !std::signbit(delta);

    float to_next = forward ? next_keyframe->time - animation_state.progress
                            : animation_state.progress - prev_keyframe->time;

    if (to_next <= 0)
    {
        bool at_end = forward
                        ? next_keyframe + 1 == animation_data.keyframes.end()
                        : prev_keyframe == animation_data.keyframes.begin();

        if (at_end)
        {
            switch (animation_state.loop_type)
            {
            case anim_loop_type::pendulum:
                new_state.rate = -animation_state.rate;
            }
        }
    }

    return new_state;
}

auto animation_system::get_keyframes(
  animation_state &animation_state,
  animation_data &animation_data)
  -> std::pair<keyframe_list::iterator, keyframe_list::iterator>
{
    if (animation_state.progress < animation_data.keyframes.front().time)
        return {
          animation_data.keyframes.begin(),
          animation_data.keyframes.begin() + 1};

    for (auto keyframe_iterator = animation_data.keyframes.begin();
         keyframe_iterator != animation_data.keyframes.end();
         ++keyframe_iterator)
    {
        if (animation_state.progress > keyframe_iterator->time)
        {
            return {keyframe_iterator, keyframe_iterator + 1};
        }
    }

    return {
      animation_data.keyframes.end() - 1, animation_data.keyframes.end() - 2};
}

void animation_system::tween_poses(
  entt::registry &registry,
  entt::entity entity,
  const pose_data &first,
  const pose_data &second,
  float distance)
{
    for (auto const &[bone_name, from, target] :
         ranges::views::zip(second.bone_names, first.targets, second.targets))
    {
        auto bone_entity = lmng::find_child(registry, entity, bone_name);
        if (bone_entity == entt::null)
        {
            throw std::runtime_error{"Entity " + bone_name + " not found"};
        }
        auto transform = registry.get<lmng::transform>(bone_entity);

        transform.position =
          from.position + distance * (target.position - from.position);

        transform.rotation = from.rotation.slerp(distance, target.rotation);

        registry.replace<lmng::transform>(bone_entity, transform);
    }
}
} // namespace lmng
