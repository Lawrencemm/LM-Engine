#include "components/character_input.h"
#include "components/enemy.h"
#include "components/protagonist.h"
#include "components/robot.h"
#include "components/robot_spawner.h"
#include <lmng/meta/reflect_component.h>

void reflect_types_cpp()
{
    REFLECT_TYPE(protagonist_component, "Protagonist")
      .REFLECT_MEMBER(protagonist_component, move_force, "Move force")
      .REFLECT_MEMBER(protagonist_component, jump_impulse, "Jump impulse");

    REFLECT_TYPE(enemy_component, "Enemy");

    REFLECT_TYPE(character_input, "Character Input")
      .REFLECT_MEMBER(character_input, radians_per_dot, "Radians per dot");

    REFLECT_TYPE(robot, "Robot");

    REFLECT_TYPE(robot_spawner_component, "Robot spawner")
      .REFLECT_MEMBER(robot_spawner_component, rows, "Rows")
      .REFLECT_MEMBER(robot_spawner_component, columns, "Columns")
      .REFLECT_MEMBER(robot_spawner_component, spacing, "Spacing");
}
