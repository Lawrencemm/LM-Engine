#pragma onceen>

#include <memory>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btCharacterControllerInterface.h>
#include <Eigen/Eigen>

namespace lmng
{
struct bt_character_controller
{
    std::unique_ptr<btPairCachingGhostObject> ghost;
    std::unique_ptr<btCollisionShape> collider;
    std::unique_ptr<btCharacterControllerInterface> controller;
};
} // namespace lmng
