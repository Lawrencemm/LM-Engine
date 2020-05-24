#pragma once
#include <Eigen/Eigen>
#include <entt/entt.hpp>

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
    virtual void rotate_character(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &euler_angles) = 0;
    virtual Eigen::Vector3f get_character_velocity(
      entt::registry const &registry,
      entt::entity entity) = 0;
    virtual ~iphysics() = default;
};

using physics = std::unique_ptr<iphysics>;

physics create_physics(entt::registry &registry);
} // namespace lmng
