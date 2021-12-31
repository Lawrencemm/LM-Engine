#pragma once
#include <Eigen/Eigen>
#include <entt/entt.hpp>
#include <fmt/format.h>

namespace lmng
{
struct box_collider
{
    Eigen::Vector3f extents{0, 0, 0};
};

struct rigid_body
{
    float mass{0.f};
    float restitution{0.f};
    float friction{0.f};
};

struct rigid_constraint
{
    entt::entity first{entt::null};
    entt::entity second{entt::null};
    float break_impulse{1.f};
};

struct broken_constraint
{
};

struct character_controller
{
    Eigen::Vector3f requested_velocity{0, 0, 0};
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
    virtual Eigen::Vector3f get_character_velocity(
      entt::registry const &registry,
      entt::entity entity) = 0;
    virtual ~iphysics() = default;
};

using physics = std::unique_ptr<iphysics>;

physics create_physics(entt::registry &registry);
} // namespace lmng

inline std::ostream &operator<<(
  std::ostream &os,
  lmng::character_controller const &character_controller)
{
    os << fmt::format(
      "Requested velocity: X: {} Y: {} Z: {}",
      character_controller.requested_velocity[0],
      character_controller.requested_velocity[1],
      character_controller.requested_velocity[2]);
    return os;
}

inline std::ostream &
  operator<<(std::ostream &os, lmng::box_collider const &box_collider)
{
    os << fmt::format(
      "X: {} Y: {} Z: {}",
      box_collider.extents[0],
      box_collider.extents[1],
      box_collider.extents[2]);
    return os;
}

inline std::ostream &
  operator<<(std::ostream &os, lmng::rigid_body const &rigid_body)
{
    os << fmt::format(
      "Mass: {} Restitution: {} Friction: {}",
      rigid_body.mass,
      rigid_body.restitution,
      rigid_body.friction);
    return os;
}
