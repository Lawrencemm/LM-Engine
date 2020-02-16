#include <catch2/catch.hpp>

#include <lmlib/math_constants.h>

#include "character_movement.h"

TEST_CASE("Swing Arms")
{
    entt::registry registry;
    auto [__, transform, controller, skeleton] = registry.create<
      lmng::transform,
      lmng::character_controller,
      character_skeleton>();

    auto [left_shoulder, left_transform] = registry.create<lmng::transform>();
    left_transform.rotation =
      Eigen::AngleAxisf{lm::pi / 2, Eigen::Vector3f::UnitX()};
    skeleton.left_shoulder = left_shoulder;

    auto [right_shoulder, right_transform] = registry.create<lmng::transform>();
    right_transform.rotation =
      Eigen::AngleAxisf{lm::pi / 2, Eigen::Vector3f::UnitX()};
    skeleton.right_shoulder = right_shoulder;

    auto character = character_movement::get_character(registry);

    auto pass_time = [&](float time) {
        while (time > 0)
        {
            float step = 0.05f;
            time -= step;
            if (time < 0)
                step += time;

            character_movement::apply_animation(character, registry, step);
        }
    };

    REQUIRE(std::holds_alternative<character_skeleton::still>(
      character.skeleton.state));

    character.controller.requested_velocity = Eigen::Vector3f::UnitZ();

    pass_time(1.f / 8);

    REQUIRE(std::holds_alternative<character_skeleton::swing>(
      character.skeleton.state));
    REQUIRE(std::get<character_skeleton::swing>(character.skeleton.state)
              .left_forward);

    character.controller.requested_velocity = Eigen::Vector3f::Zero();

    pass_time(1.f / 16);

    REQUIRE(std::holds_alternative<character_skeleton::relax>(
      character.skeleton.state));
    REQUIRE(std::get<character_skeleton::relax>(character.skeleton.state)
              .left_forward);

    pass_time(1.f / 8);

    REQUIRE(std::holds_alternative<character_skeleton::still>(
      character.skeleton.state));
}
