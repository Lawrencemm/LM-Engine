#pragma once

#include "transform.h"
#include <entt/fwd.hpp>
#include <yaml-cpp/yaml.h>

namespace lmng
{
ENTT_OPAQUE_TYPE(pose, uint32_t);
ENTT_OPAQUE_TYPE(animation, uint32_t);

enum class anim_loop_type
{
    pendulum
};

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

    pose load_pose(
      entt::registry const &registry,
      std::string const &pose_name,
      YAML::Node const &yaml);

    animation load_animation(YAML::Node const &yaml);

  private:
    void update_animation(
      entt::registry &registry,
      entt::entity entity,
      animation_state &animation_state,
      float dt);

    struct pose_data
    {
        std::vector<std::string> bone_names;
        std::vector<lmng::transform> targets;
    };

    static void tween_poses(
      entt::registry &registry,
      entt::entity entity,
      pose_data const &first,
      pose_data const &second,
      float distance);

    struct keyframe
    {
        lmng::pose pose;
        float time;
    };

    using keyframe_list = std::vector<keyframe>;

    struct animation_data
    {
        keyframe_list keyframes;
    };

    std::pair<keyframe_list::iterator, keyframe_list::iterator> get_keyframes(
      animation_state &animation_state,
      animation_data &animation_data);

    animation_state advance_animation(
      animation_state const animation_state,
      animation_system::animation_data const &animation_data,
      keyframe_list::iterator const &prev_keyframe,
      keyframe_list::iterator const &next_keyframe,
      float delta);

    unsigned next_pose_index{0};
    unsigned next_animation_index{0};

    std::unordered_map<pose, pose_data> poses;
    std::unordered_map<animation, animation_data> animations;

    std::unordered_map<std::string, pose> pose_name_to_id;
};
} // namespace lmng
