#include <lmng/loaders/animation.h>
#include <lmng/asset_cache.h>

namespace lmng
{
std::shared_ptr<pose_data>
  yaml_pose_loader::load(asset_cache &cache, std::string const &asset_path)
{
    auto yaml = YAML::LoadFile(
      (cache.content_dir / (asset_path + ".lpose")).string());

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

std::shared_ptr<animation_data>
  yaml_animation_loader::load(asset_cache &cache, const std::string &asset_path)
{
    auto yaml = YAML::LoadFile(
      (cache.content_dir / (asset_path + ".lmanim")).string());

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
}
