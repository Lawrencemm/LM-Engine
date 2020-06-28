#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Eigen/Eigen>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <memory>

namespace lmng
{
struct bt_character_controller
{
    std::unique_ptr<btPairCachingGhostObject> ghost;
    std::unique_ptr<btCollisionShape> collider;
    std::unique_ptr<btKinematicCharacterController> controller;
};
} // namespace lmng
