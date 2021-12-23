#pragma once

#include "transform.h"
#include <entt/fwd.hpp>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace lmng
{
enum class anim_loop_type
{
    pendulum
};

struct pose_data
{
    std::vector<std::string> bone_names;
    std::vector<lmng::transform> targets;
};

using pose = std::shared_ptr<pose_data>;

struct keyframe_data
{
    lmng::pose pose;
    float time;
};

using keyframe_list = std::vector<keyframe_data>;

struct animation_data
{
    keyframe_list keyframes;
};

using animation = std::shared_ptr<animation_data>;

struct animation_state
{
    lmng::animation animation;
    float rate, progress;
    anim_loop_type loop_type;
};

class animation_system
{
  public:
    void update(entt::registry &registry, float dt);

    void animate(
      entt::registry &registry,
      entt::entity skeleton_root,
      animation animation,
      float progress,
      float rate,
      anim_loop_type loop_type);

  private:
    void update_animation(
      entt::registry &registry,
      entt::entity entity,
      animation_state &animation_state,
      float dt);

    static void tween_poses(
      entt::registry &registry,
      entt::entity entity,
      pose_data const &first,
      pose_data const &second,
      float distance);

    std::pair<keyframe_list::iterator, keyframe_list::iterator> get_keyframes(
      animation_state &animation_state,
      animation_data &animation_data);

    animation_state advance_animation(
      animation_state const animation_state,
      animation_data const &animation_data,
      keyframe_list::iterator const &prev_keyframe,
      keyframe_list::iterator const &next_keyframe,
      float delta);
};
} // namespace lmng
