#include <entt/entt.hpp>
#include <lmengine/transform.h>
#include <lmlib/eigen.h>

namespace lmng
{
void look_at(transform &from, transform const &to)
{
    from.rotation = lm::pitch_yaw_quat(from.position, to.position);
}

template <typename lhs_type, typename rhs_type>
static transform compose_transforms(lhs_type const &lhs, rhs_type const &rhs)
{
    return transform{lhs.position + lhs.rotation * rhs.position,
                     lhs.rotation * rhs.rotation};
}

transform resolve_transform(entt::registry const &registry, entt::entity entity)
{
    auto &transform = registry.get<lmng::transform>(entity);

    auto maybe_parent = registry.try_get<transform_parent>(entity);

    if (maybe_parent)
    {
        return compose_transforms(
          resolve_transform(registry, maybe_parent->entity), transform);
    }

    return transform;
}

transform get_frame(entt::registry const &registry, entt::entity entity)
{
    auto maybe_parent = registry.try_get<transform_parent>(entity);
    if (maybe_parent)
        return resolve_transform(registry, maybe_parent->entity);

    return transform{Eigen::Vector3f::Zero(), Eigen::Quaternionf{}};
}

void set_transform_parent(
  entt::registry &registry,
  entt::entity entity,
  entt::entity parent)
{
    auto entity_transform = lmng::resolve_transform(registry, entity);
    auto parent_transform = lmng::resolve_transform(registry, parent);

    registry.assign_or_replace<lmng::transform_parent>(
      entity, lmng::transform_parent{parent});

    registry.replace<lmng::transform>(
      entity,
      lmng::transform{
        parent_transform.rotation.inverse() *
          (entity_transform.position - parent_transform.position),
        entity_transform.rotation * parent_transform.rotation.inverse(),
      });
}
} // namespace lmng
