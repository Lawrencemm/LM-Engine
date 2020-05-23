#include <range/v3/algorithm/find.hpp>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>

#include <lmng/transform.h>

#include "bullet_physics.h"
#include "bullet_util.h"
#include "character_controller.h"
#include "rigid_body.h"

namespace lmng
{
void world_tick_callback(btDynamicsWorld *world, btScalar dt)
{
    auto bt_physics_instance =
      static_cast<bt_physics *>(world->getWorldUserInfo());

    bt_physics_instance->on_tick(world, dt);
}

bt_physics::bt_physics(entt::registry &registry)
{
    create_broadphase();
    create_collision_config();
    create_collision_dispatcher();
    create_constraint_solver();

    create_world();

    check_invalid_colliders(registry);

    check_invalid_rigid_bodies(registry);

    create_rigid_bodies(registry);

    check_invalid_character_controllers(registry);

    create_character_controllers(registry);
}

void bt_physics::check_invalid_colliders(entt::registry &registry) const
{
    throw_invalid_entities(
      registry,
      registry.view<box_collider>(entt::exclude<transform>),
      "Colliders found with no transforms:");
}

void bt_physics::create_world()
{
    physics_world = std::make_unique<btDiscreteDynamicsWorld>(
      dispatcher.get(),
      broadphase.get(),
      solver.get(),
      collision_configuration.get());
    physics_world->setInternalTickCallback(world_tick_callback);
}

void bt_physics::create_constraint_solver()
{
    solver = std::make_unique<btSequentialImpulseConstraintSolver>();
}

void bt_physics::create_collision_dispatcher()
{
    dispatcher =
      std::make_unique<btCollisionDispatcher>(collision_configuration.get());
}

void bt_physics::create_collision_config()
{
    collision_configuration =
      std::make_unique<btDefaultCollisionConfiguration>();
}

void bt_physics::create_broadphase()
{
    broadphase = std::make_unique<btDbvtBroadphase>();
    broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(
      new btGhostPairCallback());
}

void bt_physics::step(entt::registry &entities, float dt)
{
    set_character_velocity_intervals(entities, dt);

    integrate(dt);

    sync_rigid_body_transforms(entities);

    sync_character_transforms(entities);
}

void bt_physics::integrate(float dt)
{
    physics_world->setWorldUserInfo(this);
    physics_world->stepSimulation(dt, 10, 1.f / 144.f);
    physics_world->setWorldUserInfo(nullptr);
}

std::unique_ptr<btBoxShape>
  bt_physics::create_bt_box_shape(box_collider const &box_collider) const
{
    return std::make_unique<btBoxShape>(vec_to_bt(box_collider.extents));
}

void bt_physics::set_gravity(Eigen::Vector3f const &vector)
{
    physics_world->setGravity({vector[0], vector[1], vector[2]});
}

void bt_physics::on_tick(btDynamicsWorld *world, btScalar dt) {}

bool bt_physics::is_touched(entt::registry &registry, entt::entity entity)
{
    for (auto &manifold : manifolds(physics_world.get()))
    {
        std::array entities{
          manifold.getBody0()->getUserIndex(),
          manifold.getBody1()->getUserIndex(),
        };
        if (ranges::find(entities, int(entity)) != entities.end())
        {
            if (manifold.getNumContacts() >= 1)
            {
                return true;
            }
        }
    }
    return false;
}

std::unique_ptr<iphysics> create_physics(entt::registry &registry)
{
    return std::make_unique<bt_physics>(registry);
}
} // namespace lmng
