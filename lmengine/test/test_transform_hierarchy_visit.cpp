#include <catch2/catch.hpp>
#include <entt/entt.hpp>
#include <lmengine/transform.h>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/find.hpp>

TEST_CASE("Visit transform children")
{
    entt::registry registry;
    std::array<entt::entity, 7> entities{entt::null};

    registry.create(entities.begin(), entities.end());
    registry.assign<lmng::transform_parent>(
      entities.begin() + 1,
      entities.end() - 1,
      std::array<lmng::transform_parent, 5>{
        entities[0], entities[1], entities[2], entities[0], entities[6]}
        .begin());

    std::array<bool, 7> visited{false};

    lmng::visit_transform_children(registry, entities[0], [&](auto entity) {
        visited[ranges::find(entities, entity) - entities.begin()] = true;
    });

    lmng::visit_transform_children(registry, entities[2], [&](auto entity) {
        visited[ranges::find(entities, entity) - entities.begin()] = true;
    });

    CAPTURE(visited);

    REQUIRE(ranges::equal(
      visited, std::array{false, true, false, true, true, false, false}));
}
