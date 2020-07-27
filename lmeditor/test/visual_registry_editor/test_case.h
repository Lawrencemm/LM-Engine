#pragma once

#include "../../src/controller/visual_registry_editor/visual_registry_controller.h"
#include <catch2/catch.hpp>
#include <entt/entity/registry.hpp>

class visual_registry_controller_test_case
{
  protected:
    entt::registry map;
    lmeditor::visual_registry_controller controller;

  public:
    visual_registry_controller_test_case()
        : controller{
            map,
            lmeditor::orbital_camera_init{
              .fov = (float)M_PI / 3,
              .aspect = (float)16.f / 9.f,
              .near_clip = 0.1f,
              .far_clip = 1000.f,
              .position = Eigen::Vector3f{0.f, 10.f, -25.f},
              .target = Eigen::Vector3f{0.f, 0.f, 0.f},
            },
          }
    {
    }
};
