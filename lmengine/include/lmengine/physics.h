#pragma once
#include <Eigen/Eigen>
#include <entt/entt.hpp>

#include "named_type.h"

namespace lmng
{
struct box_collider
{
    Eigen::Vector3f extents;
};

struct rigid_body
{
    float mass;
    float restitution;
    float friction;
};

struct character_controller
{
    Eigen::Vector3f requested_velocity;
};

class iphysics
{
  public:
    virtual void step(entt::registry &entities, float dt) = 0;
    virtual void set_gravity(Eigen::Vector3f const &vector) = 0;
    virtual void apply_impulse(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &vector) = 0;
    virtual void apply_force(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &vector) = 0;
    virtual bool is_touched(entt::registry &registry, entt::entity entity) = 0;
    virtual void rotate_character(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &euler_angles) = 0;
    virtual ~iphysics() = default;
};

using physics = std::unique_ptr<iphysics>;

physics create_physics(entt::registry &registry);
} // namespace lmng

LMNG_NAMED_TYPE(lmng::rigid_body, "Rigid Body");
LMNG_NAMED_TYPE(lmng::character_controller, "Character Controller");
LMNG_NAMED_TYPE(lmng::box_collider, "Box Collider");
