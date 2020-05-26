#include <entt/entt.hpp>
#include <lmlib/eigen.h>
#include <lmng/hierarchy.h>
#include <lmng/transform.h>

namespace lmng
{
void look_at(transform &from, transform const &to)
{
    from.rotation = lm::pitch_yaw_quat(from.position, to.position);
}

transform compose_transforms(transform const &lhs, transform const &rhs)
{
    return transform{
      lhs.position + lhs.rotation * rhs.position, lhs.rotation * rhs.rotation};
}

transform resolve_transform(entt::registry const &registry, entt::entity entity)
{
    auto &transform = registry.get<lmng::transform>(entity);

    auto maybe_parent = registry.try_get<parent>(entity);

    if (maybe_parent)
    {
        return compose_transforms(
          resolve_transform(registry, maybe_parent->entity), transform);
    }

    return transform;
}

transform get_frame(entt::registry const &registry, entt::entity entity)
{
    auto maybe_parent = registry.try_get<parent>(entity);
    if (maybe_parent)
        return resolve_transform(registry, maybe_parent->entity);

    return transform{Eigen::Vector3f::Zero(), Eigen::Quaternionf{}};
}

} // namespace lmng
