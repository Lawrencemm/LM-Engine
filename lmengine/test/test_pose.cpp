#include <catch2/catch.hpp>
#include <iostream>
#include <lmengine/name.h>
#include <lmengine/pose.h>
#include <lmengine/reflection.h>
#include <lmengine/serialisation.h>
#include <lmengine/transform.h>
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/view/zip.hpp>

TEST_CASE("Save/load pose")
{
    lmng::reflect_types();

    entt::registry registry;
    auto parent = registry.create();

    std::array<entt::entity, 6> children{entt::null};
    std::array<lmng::name, 6> names{"1", "2", "3", "4", "5", "6"};
    std::array<lmng::transform, 6> transforms{};

    ranges::generate(transforms, [xpos = 0.f]() mutable {
        return lmng::transform{Eigen::Vector3f{xpos += 1.f, 0.f, 0.f}};
    });

    registry.create(children.begin(), children.end());

    YAML::Node pose_yaml;

    for (auto [child, name, transform] :
         ranges::views::zip(children, names, transforms))
    {
        registry.assign<lmng::transform_parent>(child, parent);
        registry.assign<lmng::transform>(child);
        registry.assign<lmng::name>(child, name);

        pose_yaml[name.string]["Transform"] =
          lmng::serialise_component(registry, transform);
    }

    for (auto child : children)
    {
        auto child_transform = registry.get<lmng::transform>(child);
        CHECK(child_transform.position == Eigen::Vector3f::Zero());
    }

    lmng::load_pose(registry, parent, pose_yaml);

    for (auto [child, expected_transform] :
         ranges::views::zip(children, transforms))
    {
        auto actual_transform = registry.get<lmng::transform>(child);
        CHECK(expected_transform.position == actual_transform.position);
        CHECK(
          expected_transform.rotation.vec() == actual_transform.rotation.vec());
    }
}
