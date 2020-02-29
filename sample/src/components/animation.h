#pragma once

#include <variant>

#include <Eigen/Eigen>
#include <entt/entt.hpp>

struct character_skeleton
{
    entt::entity left_shoulder{entt::null}, right_shoulder{entt::null};
    float arm_swing_speed{1.f}, arm_swing_height{1.f};

    struct still
    {
    };
    struct swing
    {
        bool left_forward;
    };
    struct relax
    {
        bool left_forward;
    };

    std::variant<still, swing, relax> state{still{}};
};
