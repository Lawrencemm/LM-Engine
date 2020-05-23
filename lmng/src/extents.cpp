#include <Eigen/Eigen>
#include <entt/entt.hpp>
#include <lmng/physics.h>
#include <lmng/shapes.h>

namespace lmng
{
Eigen::Vector3f get_extents(entt::registry const &registry, entt::entity entity)
{
    auto maybe_box_render = registry.try_get<lmng::box_render>(entity);
    if (maybe_box_render)
        return maybe_box_render->extents;

    auto maybe_box_collider = registry.try_get<lmng::box_collider>(entity);
    if (maybe_box_collider)
        return maybe_box_collider->extents;

    return Eigen::Vector3f{0.f, 0.f, 0.f};
}
} // namespace lmng
