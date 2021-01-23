#include "robot_system.h"
#include "../components/character_input.h"
#include "../components/robot.h"

#include <lmlib/intersection.h>
#include <lmlib/math_constants.h>
#include <lmng/physics.h>
#include <lmng/transform.h>

void move_robots(entt::registry &registry, entt::entity ground)
{
    for (auto [entity, character_controller, transform] :
         registry
           .view<lmng::character_controller, lmng::transform, robot>(
             entt::exclude<character_input>)
           .proxy())
    {
        auto ground_extents = registry.get<lmng::box_collider>(ground).extents;
        auto ground_transform = registry.get<lmng::transform>(ground);

        Eigen::Vector2f ground_extents2{ground_extents[0], ground_extents[2]};
        Eigen::Vector2f pos_ground_relative{
          transform.position.x() - ground_transform.position.x(),
          transform.position.z() - ground_transform.position.z()};

        if (boost::geometry::within(
              pos_ground_relative, lm::get_box(ground_extents2)))
        {
            auto [segment, distance] =
              lm::closest_edge(pos_ground_relative, ground_extents2);

            auto projected = transform.rotation * Eigen::Vector3f::UnitZ();
            if (
              distance <= 1.f &&
              boost::geometry::intersects(
                segment,
                lm::segment2f{
                  pos_ground_relative,
                  pos_ground_relative +
                    Eigen::Vector2f{projected[0], projected[2]}}))
            {
                registry.replace<lmng::transform>(
                  entity,
                  lmng::transform{
                    transform.position,
                    transform.rotation *
                      Eigen::AngleAxisf{lm::pi, Eigen::Vector3f::UnitY()}});
            }
        }

        registry.replace<lmng::character_controller>(
          entity,
          lmng::character_controller{
            transform.rotation * Eigen::Vector3f::UnitZ()});
    }
}

void control_robots_animation(
  entt::registry &registry,
  lmng::physics &physics,
  lmng::animation_system &animation_system,
  lmng::animation const &animation)
{
    for (auto [entity, character_controller] :
         registry.view<lmng::character_controller, robot>().proxy())
    {
        auto current_velocity =
          physics->get_character_velocity(registry, entity);

        bool was_moving = current_velocity.squaredNorm() > 0.001f;

        bool started_moving =
          !was_moving &&
          character_controller.requested_velocity.squaredNorm() > 0.001f;

        bool stopped_moving =
          was_moving &&
          character_controller.requested_velocity.squaredNorm() < 0.001f;

        if (!was_moving)
        {
            auto maybe_animation_state =
              registry.try_get<lmng::animation_state>(entity);

            if (maybe_animation_state)
            {
                auto &animation_state = *maybe_animation_state;

                bool left_forward = animation_state.progress >= 0.5f;

                if (
                  (animation_state.rate < 0.f && !left_forward) ||
                  (animation_state.rate > 0.f && left_forward))
                {
                    registry.remove<lmng::animation_state>(entity);
                }
            }
        }

        if (started_moving)
        {
            auto maybe_animation_state =
              registry.try_get<lmng::animation_state>(entity);

            if (maybe_animation_state)
            {
                auto &animation_state = *maybe_animation_state;

                bool left_forward = animation_state.progress >= 0.5f;

                animation_state.rate =
                  std::abs(animation_state.rate) * (left_forward ? 1.f : -1.f);
            }
            else
            {
                animation_system.animate(
                  registry,
                  entity,
                  animation,
                  0.5f,
                  1.f,
                  lmng::anim_loop_type::pendulum);
            }
        }

        if (stopped_moving)
        {
            auto &animation_state = registry.get<lmng::animation_state>(entity);

            bool left_forward = animation_state.progress >= 0.5f;

            animation_state.rate =
              std::abs(animation_state.rate) * (left_forward ? -1.f : 1.f);
        }
    }
}
