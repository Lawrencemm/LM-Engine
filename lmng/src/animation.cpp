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

yaml_pose_loader::yaml_pose_loader(const std::filesystem::path &project_dir)
    : project_dir{project_dir}
{
}

std::shared_ptr<pose_data>
  yaml_pose_loader::load(asset_cache &cache, std::string const &asset_path)
{
    auto yaml = YAML::LoadFile(
      (project_dir / "assets" / (asset_path + ".lpose")).string());

    pose_data data;

    for (auto const &yaml_entity : yaml)
    {
        transform target_transform;

        auto transform_yaml = yaml_entity.second["Transform"];

        auto pos_strstr =
          std::istringstream{transform_yaml["Position"].as<std::string>()};

        pos_strstr >> target_transform.position[0] >>
          target_transform.position[1] >> target_transform.position[2];

        auto rot_strstr =
          std::istringstream{transform_yaml["Rotation"].as<std::string>()};

        rot_strstr >> target_transform.rotation.coeffs()[0] >>
          target_transform.rotation.coeffs()[1] >>
          target_transform.rotation.coeffs()[2] >>
          target_transform.rotation.coeffs()[3];

        data.bone_names.emplace_back(yaml_entity.first.as<std::string>());
        data.targets.emplace_back(target_transform);
    }

    return std::make_shared<pose_data>(std::move(data));
}

yaml_animation_loader::yaml_animation_loader(
  const std::filesystem::path &project_dir)
    : project_dir{project_dir}
{
}

std::shared_ptr<animation_data>
  yaml_animation_loader::load(asset_cache &cache, const std::string &asset_path)
{
    auto yaml = YAML::LoadFile(
      (project_dir / "assets" / (asset_path + ".lmanim")).string());

    animation_data data;

    for (auto const &keyframe_yaml : yaml["Keyframes"])
    {
        data.keyframes.emplace_back(keyframe_data{
          cache.load<lmng::pose_data>(keyframe_yaml["Pose"].as<std::string>()),
          keyframe_yaml["Time"].as<float>(),
        });
    }

    return std::make_shared<animation_data>(std::move(data));
}
} // namespace lmng
