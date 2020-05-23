#include <catch2/catch.hpp>
#include <entt/entt.hpp>

#include <lmng/transform.h>

TEST_CASE("Transform Hierarchy")
{
    Eigen::Vector3f parent_position{1.f, 2.f, 3.f};
    Eigen::Quaternionf parent_rotation{
      Eigen::AngleAxisf{M_PI_2, Eigen::Vector3f::UnitX()}};

    Eigen::Vector3f child_position{4.f, 5.f, 6.f};
    Eigen::Quaternionf child_rotation{
      Eigen::AngleAxisf{M_PI_2, Eigen::Vector3f::UnitY()}};

    entt::registry registry;
    auto parent = registry.create();
    auto parent_transform = registry.assign<lmng::transform>(
      parent, parent_position, parent_rotation);

    auto child = registry.create();
    auto child_transform =
      registry.assign<lmng::transform>(child, child_position, child_rotation);

    auto grandchild = registry.create();
    auto grandchild_transform = registry.assign<lmng::transform>(
      grandchild, child_position, child_rotation);

    registry.assign<lmng::transform_parent>(child, parent);
    registry.assign<lmng::transform_parent>(grandchild, child);

    lmng::transform expected_transform{
      parent_transform.position +
        parent_transform.rotation * child_transform.position,
      parent_transform.rotation * child_transform.rotation};

    auto child_actual = lmng::resolve_transform(registry, child);

    CHECK(child_actual.position == expected_transform.position);
    CHECK(child_actual.rotation.isApprox(expected_transform.rotation));

    expected_transform.position += expected_transform.rotation * child_position;
    expected_transform.rotation *= child_rotation;

    auto grandchild_actual = lmng::resolve_transform(registry, grandchild);

    CHECK(grandchild_actual.position == expected_transform.position);
    CHECK(grandchild_actual.rotation.isApprox(expected_transform.rotation));
}