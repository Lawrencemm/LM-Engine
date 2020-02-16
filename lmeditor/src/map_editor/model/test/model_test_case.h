#pragma once

#include "../model.h"
#include <catch2/catch.hpp>

class model_test_case
{
  protected:
    lmeditor::map_editor_model model;

  public:
    model_test_case()
        : model{lmeditor::map_editor_model::init{
            .camera_init = lmeditor::orbital_camera_init{
              .fov = (float)M_PI / 3,
              .aspect = (float)16.f / 9.f,
              .near_clip = 0.1f,
              .far_clip = 1000.f,
              .position = Eigen::Vector3f{0.f, 10.f, -25.f},
              .target = Eigen::Vector3f{0.f, 0.f, 0.f},
            }}}
    {
    }
};
