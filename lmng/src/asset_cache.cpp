#include <lmng/asset_cache.h>
#include "lmng/loaders/archetype.h"
#include "lmng/loaders/animation.h"

namespace lmng
{
asset_cache::asset_cache(std::filesystem::path content_dir)
  : content_dir{std::move(content_dir)}
{
    emplace_loader<yaml_pose_loader>();
    emplace_loader<yaml_animation_loader>();
    emplace_loader<yaml_archetype_loader>();
}
}
