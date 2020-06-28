#include <catch2/catch.hpp>
#include <iostream>
#include <lmlib/range.h>
#include <lmng/hierarchy.h>
#include <lmng/meta/any_component.h>
#include <lmng/name.h>
#include <lmng/pose.h>
#include <lmng/serialisation.h>
#include <lmng/transform.h>
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/view/zip.hpp>

std::pair<entt::entity, std::array<entt::entity, 4>>
  create_test_hierarchy(entt::registry &registry)
{
    auto parent = registry.create();

    std::array<entt::entity, 4> children{entt::null};

    registry.create(children.begin(), children.end());

    registry.assign<lmng::name>(
      children.begin(),
      children.end(),
      std::array<lmng::name, 4>{"1", "11", "111", "2"}.begin());

    registry.assign<lmng::parent>(
      children.begin(),
      children.end(),
      std::array<lmng::parent, 4>{parent, children[0], children[1], parent}
        .begin());

    return {parent, children};
};

TEST_CASE("Save/load pose")
{
    entt::registry registry;
    lmng::hierarchy_system hierarchy_system{registry};

    auto [parent, children] = create_test_hierarchy(registry);

    std::array transforms{
      lmng::transform{{1.f, 0.f, 0.f}},
      lmng::transform{{1.f, 0.f, 0.f}},
      lmng::transform{{1.f, 0.f, 0.f}},
      lmng::transform{{2.f, 0.f, 0.f}},
    };

    registry.assign<lmng::transform>(
      children.begin(), children.end(), transforms.begin());

    auto yaml = lmng::save_pose(registry, parent);

    auto loaded_registry = entt::registry{};

    std::tie(parent, children) = create_test_hierarchy(loaded_registry);

    loaded_registry.assign<lmng::transform>(
      children.begin(), children.end(), {});

    lmng::load_pose(loaded_registry, parent, yaml);

    loaded_registry.assign<lmng::transform>(parent);

    for (auto i : lm::range(4))
    {
        CHECK(
          loaded_registry.get<lmng::transform>(children[i]) == transforms[i]);
    }

    auto resolved_child = lmng::resolve_transform(loaded_registry, children[2]);

    auto composed_transform = lmng::compose_transforms(
      lmng::compose_transforms(transforms[0], transforms[1]), transforms[2]);

    CAPTURE(
      resolved_child.position,
      composed_transform.position,
      resolved_child.rotation,
      composed_transform.rotation);

    CHECK(resolved_child == composed_transform);
}
