#pragma once

#include <lmng/animation.h>
#include <lmng/loaders/loader.h>

namespace lmng
{
class asset_cache;

class yaml_pose_loader : public asset_loader_interface<pose_data>
{
  public:
    std::shared_ptr<pose_data>
      load(asset_cache &cache, std::string const &asset_path) override;
};

class yaml_animation_loader : public asset_loader_interface<animation_data>
{
  public:
    std::shared_ptr<animation_data>
      load(asset_cache &cache, std::string const &asset_path) override;
};
}
