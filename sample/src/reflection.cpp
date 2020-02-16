#include "components/animation.h"
#include "components/character_input.h"
#include "components/enemy.h"
#include "components/protagonist.h"

#include <lmengine/reflection.h>

void reflect_types_cpp()
{
    REFLECT_TYPE(protagonist_component, "Protagonist")
      .REFLECT_MEMBER(protagonist_component, move_force, "Move force")
      .REFLECT_MEMBER(protagonist_component, jump_impulse, "Jump impulse");

    REFLECT_TYPE(enemy_component, "Enemy");

    REFLECT_TYPE(character_input, "Character Input")
      .REFLECT_MEMBER(character_input, radians_per_dot, "Radians per dot");

    REFLECT_TYPE(character_skeleton, "Character Skeleton")
      .REFLECT_MEMBER(character_skeleton, left_shoulder, "Left Shoulder")
      .REFLECT_MEMBER(character_skeleton, right_shoulder, "Right Shoulder")
      .REFLECT_MEMBER(character_skeleton, arm_swing_speed, "Arm swing speed")
      .REFLECT_MEMBER(character_skeleton, arm_swing_height, "Arm swing height");
}
