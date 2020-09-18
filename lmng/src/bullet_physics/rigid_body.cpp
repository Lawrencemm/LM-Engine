#include "rigid_body.h"

#include <LinearMath/btDefaultMotionState.h>

#include "bullet_physics.h"
#include "bullet_util.h"

namespace lmng
{
bt_rigid_body bt_physics::create_box_rigid_body(
  entt::entity entity,
  box_collider const &box_collider,
  const transform &transform,
  rigid_body const &abstract_rigid_body)
{
    auto box = create_bt_box_shape(box_collider);

    auto box_motion_state = std::make_unique<btDefaultMotionState>(btTransform{
      quat_to_bt(transform.rotation),
      vec_to_bt(transform.position),
    });

    btVector3 inertia;

    box->calculateLocalInertia(abstract_rigid_body.mass, inertia);

    auto box_body =
      std::make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo(
        abstract_rigid_body.mass, box_motion_state.get(), box.get(), inertia));

    box_body->setRestitution(abstract_rigid_body.restitution);
    box_body->setFriction(abstract_rigid_body.friction);
    box_body->setUserIndex(int(entity));

    physics_world->addRigidBody(box_body.get());

    return bt_rigid_body{
      .shape = std::move(box),
      .motion_state = std::move(box_motion_state),
      .rigid_body = std::move(box_body),
    };
}

void bt_physics::check_invalid_rigid_bodies(entt::registry &registry) const
{
    throw_invalid_entities(
      registry,
      registry.view<rigid_body>(entt::exclude<lmng::box_collider>),
      "Rigid bodies with no colliders found:");
}

void bt_physics::create_rigid_bodies(entt::registry &registry)
{
    registry.view<transform, rigid_body, box_collider>().each(
      [&](
        auto entity,
        auto const &transform,
        auto const &rigid_body,
        auto const &box_collider) {
          registry.emplace<bt_rigid_body>(
            entity,
            create_box_rigid_body(entity, box_collider, transform, rigid_body));
      });
}

void bt_physics::sync_rigid_body_transforms(entt::registry &entities) const
{
    entities.view<transform, bt_rigid_body>().each(
      [&](auto entity, transform &transform, bt_rigid_body &rigid_body) {
          auto bt_transform = rigid_body.rigid_body->getWorldTransform();
          auto origin = bt_transform.getOrigin();
          transform.position[0] = origin.getX();
          transform.position[1] = origin.getY();
          transform.position[2] = origin.getZ();

          auto rotation = bt_transform.getRotation();
          transform.rotation = Eigen::Quaternionf{
            rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()};
      });
}

void bt_physics::apply_impulse(
  entt::registry &registry,
  entt::entity entity,
  Eigen::Vector3f const &vector)
{
    bt_rigid_body &rigid_body = registry.get<bt_rigid_body>(entity);
    rigid_body.rigid_body->activate();
    rigid_body.rigid_body->applyCentralImpulse(vec_to_bt(vector));
}

void bt_physics::apply_force(
  entt::registry &registry,
  entt::entity entity,
  Eigen::Vector3f const &vector)
{
    bt_rigid_body &rigid_body = registry.get<bt_rigid_body>(entity);
    rigid_body.rigid_body->activate();
    rigid_body.rigid_body->applyCentralForce(vec_to_bt(vector));
}
} // namespace lmng
