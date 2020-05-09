#include "rigid_bodies_battle.h"
#include "../components/enemy.h"
#include "../components/protagonist.h"
#include <lmengine/transform.h>
#include <lmlib/variant_visitor.h>

rigid_bodies_battle::rigid_bodies_battle(lmng::simulation_init const &init)
    : physics{lmng::create_physics(init.registry)}
{
    physics->set_gravity({0.f, -20.f, 0.f});
}

void rigid_bodies_battle::handle_input_event(
  lmtk::input_event const &input_event,
  entt::registry &registry)
{
    auto protagonist_view = registry.view<protagonist_component>();
    if (!protagonist_view.empty())
    {
        auto protagonist = protagonist_view[0];
        auto &protagonist_component = protagonist_view.get(protagonist);
        float jump_impulse = protagonist_component.jump_impulse;
        input_event >>
          lm::variant_visitor{
            [&](lmtk::key_down_event const &key_down_event) {
                switch (key_down_event.key)
                {
                case lmpl::key_code::Space:
                    if (!physics->is_touched(registry, protagonist))
                        return false;

                    physics->apply_impulse(
                      registry, protagonist, {0.f, jump_impulse, 0.f});
                    return true;

                default:
                    break;
                }
                return false;
            },
            [](auto) { return false; },
          };
    }
}

void rigid_bodies_battle::update(
  entt::registry &registry,
  float dt,
  lmtk::input_state const &input_state)
{
    auto protagonist = registry.view<struct protagonist_component>()[0];

    apply_movement_controls(registry, input_state, protagonist);

    move_enemies(registry, protagonist);

    physics->step(registry, dt);
}

void rigid_bodies_battle::apply_movement_controls(
  entt::registry &registry,
  lmtk::input_state const &input_state,
  entt::entity protagonist) const
{
    if (!physics->is_touched(registry, protagonist))
        return;

    auto &protagonist_component =
      registry.get<struct protagonist_component>(protagonist);
    auto move_force = protagonist_component.move_force;

    if (input_state.key_state[lmpl::key_code::S])
        physics->apply_force(registry, protagonist, {-move_force, 0.f, 0.f});

    if (input_state.key_state[lmpl::key_code::F])
        physics->apply_force(registry, protagonist, {move_force, 0.f, 0.f});

    if (input_state.key_state[lmpl::key_code::E])
        physics->apply_force(registry, protagonist, {0.f, 0.f, move_force});

    if (input_state.key_state[lmpl::key_code::D])
        physics->apply_force(registry, protagonist, {0.f, 0.f, -move_force});
}

void rigid_bodies_battle::move_enemies(
  entt::registry &registry,
  entt::entity protagonist) const
{
    auto const &protagonist_component =
      registry.get<struct protagonist_component>(protagonist);
    auto const &protagonist_transform =
      registry.get<lmng::transform>(protagonist);

    auto enemy_view =
      registry.view<enemy_component, lmng::rigid_body, lmng::transform>();

    enemy_view.each([&](auto enemy, auto, auto &rigid_body, auto &transform) {
        if (!physics->is_touched(registry, enemy))
            return;

        Eigen::Vector3f to_protag =
          (protagonist_transform.position - transform.position).normalized();
        physics->apply_force(
          registry, enemy, protagonist_component.move_force * to_protag);
    });
}
