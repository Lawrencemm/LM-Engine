#include "bullet_physics.h"
#include "bullet_util.h"
#include <BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h>

namespace lmng
{
struct bt_rigid_constraint
{
    std::unique_ptr<btTypedConstraint> bt_constraint;
};

void bt_physics::update_constraints(entt::registry &registry, float dt)
{
    for (auto entity : registry.view<lmng::transform, lmng::rigid_constraint>(
           entt::exclude<bt_rigid_constraint>))
    {
        add_new_bt_constraint(registry, entity);
    }
}

void bt_physics::sync_constraints(entt::registry &registry)
{
    for (auto [entity, bt_constraint] :
         registry.view<bt_rigid_constraint>().proxy())
    {
        if (!bt_constraint.bt_constraint->isEnabled())
        {
            registry.emplace<lmng::broken_constraint>(entity);
        }
        else
        {
            registry.remove_if_exists<lmng::broken_constraint>(entity);
        }
    }
}

void bt_physics::add_new_bt_constraint(
  entt::registry &registry,
  entt::entity entity)
{
    auto constraint_handle = entt::handle{registry, entity};
    auto constraint = constraint_handle.get<rigid_constraint>();
    auto constraint_pos = constraint_handle.get<lmng::transform>().position;

    auto first_handle = entt::handle{registry, constraint.first};
    auto second_handle = entt::handle{registry, constraint.second};

    Eigen::Vector3f pos_a =
      constraint_pos - first_handle.get<lmng::transform>().position;
    Eigen::Vector3f pos_b =
      constraint_pos - second_handle.get<lmng::transform>().position;

    bt_rigid_constraint &bt_constraint =
      constraint_handle.emplace<bt_rigid_constraint>(
        std::make_unique<btGeneric6DofConstraint>(
          *first_handle.get<bt_rigid_body>().rigid_body,
          *second_handle.get<bt_rigid_body>().rigid_body,
          bt_transform(lmng::transform{pos_a}),
          bt_transform(lmng::transform{pos_b}),
          true));

    bt_constraint.bt_constraint->setBreakingImpulseThreshold(
      constraint.break_impulse);
}
} // namespace lmng
