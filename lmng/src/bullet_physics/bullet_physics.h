#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <range/v3/algorithm/copy.hpp>

#include <lmng/name.h>
#include <lmng/physics.h>
#include <lmng/transform.h>

#include "rigid_body.h"

namespace lmng
{
class bt_physics : public iphysics
{
  public:
    explicit bt_physics(entt::registry &registry);
    void step(entt::registry &entities, float dt) override;
    bt_rigid_body create_box_rigid_body(
      entt::entity entity,
      box_collider const &box_shape,
      const transform &transform,
      rigid_body const &abstract_rigid_body);
    btDynamicsWorld *operator->() { return physics_world.get(); }
    void set_gravity(Eigen::Vector3f const &vector) override;
    void apply_impulse(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &vector) override;
    void apply_force(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &vector) override;
    void on_tick(btDynamicsWorld *world, btScalar dt);
    bool is_touched(entt::registry &registry, entt::entity entity) override;
    void rotate_character(
      entt::registry &registry,
      entt::entity entity,
      Eigen::Vector3f const &euler_angles) override;

    void create_broadphase();
    void create_collision_config();
    void create_collision_dispatcher();
    void create_constraint_solver();
    void create_world();
    void check_invalid_colliders(entt::registry &registry) const;
    void create_rigid_bodies(entt::registry &registry);
    void check_invalid_rigid_bodies(entt::registry &registry) const;
    void check_invalid_character_controllers(entt::registry &registry) const;
    void create_character_controllers(entt::registry &registry) const;

    [[nodiscard]] std::unique_ptr<btBoxShape>
      create_bt_box_shape(box_collider const &box_collider) const;

    std::unique_ptr<btPairCachingGhostObject> create_ghost_object(
      transform const &transform,
      btCollisionShape *collision_shape) const;

    std::unique_ptr<btKinematicCharacterController> create_character_controller(
      btBoxShape *collision_shape,
      btPairCachingGhostObject *ghost) const;

    void sync_character_transforms(entt::registry &entities) const;
    void sync_rigid_body_transforms(entt::registry &entities) const;
    void integrate(float dt);
    void set_character_velocity_intervals(entt::registry &entities, float dt)
      const;

  private:
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btDispatcher> dispatcher;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> physics_world;
};

template <typename view_type>
void throw_invalid_entities(
  entt::registry &registry,
  view_type const &invalid_view,
  char const *msg)
{
    std::vector<entt::entity> invalid_entities{};

    ranges::copy(invalid_view, invalid_entities.begin());

    if (!invalid_entities.empty())
    {
        std::ostringstream formatter{
          std::string{msg} + '\n',
          std::ios_base::ate,
        };

        for (auto entity : invalid_entities)
            formatter << get_name(registry, entity) << "\n";

        throw std::runtime_error{formatter.str()};
    }
}
} // namespace lmng
