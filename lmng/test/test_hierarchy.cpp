#include <Eigen/Eigen>
#include <catch2/catch.hpp>
#include <entt/entity/registry.hpp>
#include <lmlib/math_constants.h>
#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmng/transform.h>

TEST_CASE("Hierarchy system")
{
    entt::registry registry;
    lmng::hierarchy_system hierarchy_system{registry};

    auto parent = registry.create();
    auto child = registry.create();

    registry.assign<lmng::parent>(child, parent);

    unsigned counter{0};

    for (auto child : lmng::child_range{registry, parent})
    {
        counter++;
    }

    CHECK(counter == 1);

    lmng::transform transform{
      Eigen::Vector3f{1.f, 1.f, 1.f},
      Eigen::Quaternionf{
        Eigen::AngleAxisf{lm::pi / 2, Eigen::Vector3f::UnitY()}}};

    registry.assign<lmng::transform>(parent, transform);
    registry.assign<lmng::transform>(child, transform);

    auto composed_transform = lmng::compose_transforms(transform, transform);
    auto child_resolved = lmng::resolve_transform(registry, child);

    CHECK(child_resolved.position == composed_transform.position);
    CHECK(child_resolved.rotation.isApprox(composed_transform.rotation));

    SECTION("Orphan child")
    {
        lmng::orphan_children(registry, parent);
        auto child_transform = registry.get<lmng::transform>(child);
        CHECK(child_transform.position == composed_transform.position);
        CHECK(child_transform.rotation.isApprox(composed_transform.rotation));
    }

    SECTION("Reparent")
    {
        auto new_parent = registry.create();
        lmng::transform new_parent_transform{
          Eigen::Vector3f{10.f, 10.f, 10.f},
          Eigen::Quaternionf{
            Eigen::AngleAxisf{-lm::pi / 4, Eigen::Vector3f::UnitX()}}};

        registry.assign<lmng::transform>(new_parent, new_parent_transform);

        auto before_reparent_transform =
          lmng::resolve_transform(registry, child);

        lmng::reparent(registry, child, new_parent);

        auto after_reparent_transform =
          lmng::resolve_transform(registry, child);

        CHECK(after_reparent_transform.position.isApprox(
          before_reparent_transform.position));

        auto dist = after_reparent_transform.rotation.angularDistance(
          before_reparent_transform.rotation);
        CAPTURE(dist);
        CHECK(after_reparent_transform.rotation.isApprox(
          before_reparent_transform.rotation));
    }
}

TEST_CASE("Recursive hierarchy range")
{
    entt::registry registry;
    lmng::hierarchy_system hierarchy_system{registry};

    auto parent = registry.create();
    registry.assign<lmng::name>(parent, "Parent");

    auto child_1 = registry.create();
    registry.assign<lmng::name>(child_1, "Child 1");
    registry.assign<lmng::parent>(child_1, parent);

    auto child_1_child_1 = registry.create();
    registry.assign<lmng::name>(child_1_child_1, "Child 1 child 1");
    registry.assign<lmng::parent>(child_1_child_1, child_1);

    auto child_1_child_2 = registry.create();
    registry.assign<lmng::name>(child_1_child_2, "Child 1 child 2");
    registry.assign<lmng::parent>(child_1_child_2, child_1);

    auto child_2 = registry.create();
    registry.assign<lmng::name>(child_2, "Child 2");
    registry.assign<lmng::parent>(child_2, parent);

    auto child_2_child = registry.create();
    registry.assign<lmng::name>(child_2_child, "Child 2 child");
    registry.assign<lmng::parent>(child_2_child, child_2);

    unsigned counter{0};

    for (auto curr_child : lmng::recursive_child_range{registry, parent})
    {
        if (counter >= registry.size() - 1)
            FAIL("Too many entities iterated");

        registry.assign<unsigned>(curr_child, counter++);
    }

    REQUIRE(registry.has<unsigned>(child_1));
    CHECK(registry.get<unsigned>(child_1) == 0);

    REQUIRE(registry.has<unsigned>(child_1_child_1));
    CHECK(registry.get<unsigned>(child_1_child_1) == 1);

    REQUIRE(registry.has<unsigned>(child_1_child_2));
    CHECK(registry.get<unsigned>(child_1_child_2) == 2);

    REQUIRE(registry.has<unsigned>(child_2));
    CHECK(registry.get<unsigned>(child_2) == 3);

    REQUIRE(registry.has<unsigned>(child_2_child));
    CHECK(registry.get<unsigned>(child_2_child) == 4);

    auto childless = registry.create();

    unsigned childless_child_visit_count{0};
    for (auto unused : lmng::recursive_child_range{registry, childless})
    {
        childless_child_visit_count++;
    }

    REQUIRE(childless_child_visit_count == 0);
}
