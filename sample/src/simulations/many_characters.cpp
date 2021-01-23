#include "many_characters.h"
#include "../components/character_input.h"
#include "../components/robot.h"
#include "../components/robot_spawner.h"
#include "../system/robot_system.h"
#include <boost/geometry.hpp>
#include <iostream>
#include <lmlib/intersection.h>
#include <lmlib/math_constants.h>
#include <lmng/animation.h>
#include <lmng/archetype.h>
#include <lmng/camera.h>
#include <lmng/kinematic.h>
#include <lmng/name.h>
#include <lmng/simulation.h>
#include <lmng/transform.h>
#include <yaml-cpp/yaml.h>

many_characters::many_characters(lmng::simulation_init const &init)
    : physics{lmng::create_physics(init.registry)},
      camera{init.registry.create()},
      animation_system{},
      hierarchy_system{init.registry},
      ground{lmng::find_entity(init.registry, "Ground")},
      swing_arms_animation{
        init.asset_cache.load<lmng::animation_data>("animation/swing_arms")}
{
    init.registry.emplace<lmng::transform>(camera);
    init.registry.emplace<lmng::camera>(camera, 1.1f, 0.1f, 1000.f, true);

    spawn_robots(init);
}

void many_characters::spawn_robots(lmng::simulation_init const &init) const
{
    for (auto [entity, spawner, spawner_transform] :
         init.registry.view<robot_spawner_component, lmng::transform>().proxy())
    {
        float start{spawner.rows * spawner.spacing / -2};
        float xpos{start};
        for (auto row : lm::range(spawner.rows))
        {
            float zpos{start};
            for (auto column : lm::range(spawner.columns))
            {
                lmng::instantiate_archetype(
                  init.registry,
                  "archetype/robot",
                  init.asset_cache,
                  lmng::transform{
                    .position = spawner_transform.position +
                                Eigen::Vector3f{xpos, 0.f, zpos}});
                zpos += spawner.spacing;
            }
            xpos += spawner.spacing;
        }
    }
}

void many_characters::handle_input_event(
  lmtk::input_event const &input_event,
  entt::registry &registry)
{
}

void many_characters::update(
  entt::registry &registry,
  float dt,
  lmtk::input_state const &input_state)
{
    move_robots(registry, ground);

    control_robots_animation(
      registry, physics, animation_system, swing_arms_animation);

    physics->step(registry, dt);

    animation_system.update(registry, dt);
}
