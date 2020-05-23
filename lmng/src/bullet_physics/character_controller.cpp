#include "character_controller.h"

#include "bullet_physics.h"
#include "bullet_util.h"

namespace lmng
{

void bt_physics::check_invalid_character_controllers(
  entt::registry &registry) const
{
    throw_invalid_entities(
      registry,
      registry.view<character_controller>(entt::exclude<box_collider>),
      "Entities with character controllers missing transforms:");
}

void bt_physics::create_character_controllers(entt::registry &registry) const
{
    registry.view<transform, character_controller, box_collider>().each(
      [&](
        auto entity,
        auto const &transform,
        auto const &character_controller,
        auto const &box_collider) {
          auto collision_shape = create_bt_box_shape(box_collider);

          auto ghost = create_ghost_object(transform, collision_shape.get());

          auto bt_controller =
            create_character_controller(collision_shape.get(), ghost.get());

          auto &ghost_transform = ghost->getWorldTransform();
          ghost_transform.setRotation(quat_to_bt(transform.rotation));

          registry.assign<bt_character_controller>(
            entity,
            std::move(ghost),
            std::move(collision_shape),
            std::move(bt_controller));
      });
}

static Eigen::Vector3f const character_controller_up{0.f, 1.f, 0.f};

std::unique_ptr<btKinematicCharacterController>
  bt_physics::create_character_controller(
    btBoxShape *collision_shape,
    btPairCachingGhostObject *ghost) const
{
    auto bt_controller = std::make_unique<btKinematicCharacterController>(
      ghost, collision_shape, 1.f, vec_to_bt(character_controller_up));

    physics_world->addAction(bt_controller.get());
    return bt_controller;
}

std::unique_ptr<btPairCachingGhostObject> bt_physics::create_ghost_object(
  lmng::transform const &transform,
  btCollisionShape *collision_shape) const
{
    auto ghost = std::make_unique<btPairCachingGhostObject>();

    // The bullet kinematic character controller starts with an up vector of
    // 0 0 1.
    // For some reason it rotates the ghost from the hardcoded up vector to the
    // up vector given in the constructor. Here we're countering that rotation.
    Eigen::Vector3f bt_controller_hardcoded_up{0.f, 0.f, 1.f};
    auto counter_rotation = Eigen::Quaternionf::FromTwoVectors(
      character_controller_up, bt_controller_hardcoded_up);

    btTransform ghost_initial_transform{
      quat_to_bt(counter_rotation),
      vec_to_bt(transform.position),
    };
    ghost->setWorldTransform(ghost_initial_transform);

    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    ghost->setCollisionShape(collision_shape);
    physics_world->addCollisionObject(
      ghost.get(),
      btBroadphaseProxy::CharacterFilter,
      btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    return ghost;
}

void bt_physics::set_character_velocity_intervals(
  entt::registry &entities,
  float dt) const
{
    entities.view<character_controller, bt_character_controller>().each(
      [&](
        auto character,
        character_controller &controller,
        bt_character_controller &bt_controller) {
          bt_controller.controller->setVelocityForTimeInterval(
            vec_to_bt(controller.requested_velocity), dt);
      });
}

void bt_physics::sync_character_transforms(entt::registry &entities) const
{
    entities.view<transform, bt_character_controller>().each(
      [&](auto character, auto &transform, auto &bt_controller) {
          auto &controller_transform = bt_controller.ghost->getWorldTransform();
          set_from_bt(transform.position, controller_transform.getOrigin());
          set_from_bt(transform.rotation, controller_transform.getRotation());
      });
}

void bt_physics::rotate_character(
  entt::registry &registry,
  entt::entity entity,
  Eigen::Vector3f const &euler_angles)
{
    auto &controller = registry.get<bt_character_controller>(entity);
    auto xform = controller.ghost->getWorldTransform();

    auto char_rot = bt_to_quat(xform.getRotation());

    Eigen::Vector3f roll_axis = Eigen::Vector3f::UnitZ();
    Eigen::Vector3f yaw_axis = Eigen::Vector3f::UnitY();
    Eigen::Vector3f pitch_axis = (char_rot * roll_axis).cross(yaw_axis);

    char_rot = Eigen::AngleAxisf{euler_angles[2], roll_axis} * char_rot;
    char_rot = Eigen::AngleAxisf{euler_angles[0], pitch_axis} * char_rot;
    char_rot = Eigen::AngleAxisf{euler_angles[1], yaw_axis} * char_rot;

    xform.setRotation(quat_to_bt(char_rot));
    controller.ghost->setWorldTransform(xform);
}

} // namespace lmng
