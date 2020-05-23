#pragma once

#include <memory>

#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace lmng
{
struct bt_rigid_body
{
    std::unique_ptr<btCollisionShape> shape;
    std::unique_ptr<btMotionState> motion_state;
    std::unique_ptr<btRigidBody> rigid_body;
};
} // namespace lmng
