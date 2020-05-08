#include "model_test_case.h"
#include <catch2/catch.hpp>
#include <lmlib/math_constants.h>

TEST_CASE_METHOD(model_test_case, "Move selection")
{
    std::array cubes{
      model.add_cube(map, {0.f, 0.f, 0.f}, 1.f),
      model.add_cube(map, {2.f, 2.f, 2.f}, 1.f),
      model.add_cube(map, {4.f, 4.f, 4.f}, 1.f),
    };

    auto [camera_rotation, key_code, initial_selection, expected_selection] =
      GENERATE(table<float, lmpl::key_code, int, int>({
        {0.f, lmpl::key_code::J, 0, 0},
        {0.f, lmpl::key_code::L, 0, 1},
        {0.f, lmpl::key_code::I, 0, 1},
        {0.f, lmpl::key_code::K, 0, 0},
        {0.f, lmpl::key_code::U, 0, 0},
        {0.f, lmpl::key_code::O, 0, 1},
        {0.f, lmpl::key_code::J, 1, 0},
        {0.f, lmpl::key_code::L, 1, 2},
        {0.f, lmpl::key_code::I, 1, 2},
        {0.f, lmpl::key_code::K, 1, 0},
        {0.f, lmpl::key_code::U, 1, 0},
        {0.f, lmpl::key_code::O, 1, 2},
        {0.f, lmpl::key_code::J, 2, 1},
        {0.f, lmpl::key_code::L, 2, 2},
        {0.f, lmpl::key_code::I, 2, 2},
        {0.f, lmpl::key_code::K, 2, 1},
        {0.f, lmpl::key_code::U, 2, 1},
        {0.f, lmpl::key_code::O, 2, 2},
        {lm::pi / 2, lmpl::key_code::J, 0, 1},
        {lm::pi / 2, lmpl::key_code::L, 0, 0},
        {lm::pi / 2, lmpl::key_code::I, 0, 1},
        {lm::pi / 2, lmpl::key_code::K, 0, 0},
        {lm::pi / 2, lmpl::key_code::U, 0, 0},
        {lm::pi / 2, lmpl::key_code::O, 0, 1},
        {lm::pi / 2, lmpl::key_code::J, 1, 2},
        {lm::pi / 2, lmpl::key_code::L, 1, 0},
        {lm::pi / 2, lmpl::key_code::I, 1, 2},
        {lm::pi / 2, lmpl::key_code::K, 1, 0},
        {lm::pi / 2, lmpl::key_code::U, 1, 0},
        {lm::pi / 2, lmpl::key_code::O, 1, 2},
        {lm::pi / 2, lmpl::key_code::J, 2, 2},
        {lm::pi / 2, lmpl::key_code::L, 2, 1},
        {lm::pi / 2, lmpl::key_code::I, 2, 2},
        {lm::pi / 2, lmpl::key_code::K, 2, 1},
        {lm::pi / 2, lmpl::key_code::U, 2, 1},
        {lm::pi / 2, lmpl::key_code::O, 2, 2},
        {lm::pi, lmpl::key_code::J, 0, 1},
        {lm::pi, lmpl::key_code::L, 0, 0},
        {lm::pi, lmpl::key_code::I, 0, 1},
        {lm::pi, lmpl::key_code::K, 0, 0},
        {lm::pi, lmpl::key_code::U, 0, 1},
        {lm::pi, lmpl::key_code::O, 0, 0},
        {lm::pi, lmpl::key_code::J, 1, 2},
        {lm::pi, lmpl::key_code::L, 1, 0},
        {lm::pi, lmpl::key_code::I, 1, 2},
        {lm::pi, lmpl::key_code::K, 1, 0},
        {lm::pi, lmpl::key_code::U, 1, 2},
        {lm::pi, lmpl::key_code::O, 1, 0},
        {lm::pi, lmpl::key_code::J, 2, 2},
        {lm::pi, lmpl::key_code::L, 2, 1},
        {lm::pi, lmpl::key_code::I, 2, 2},
        {lm::pi, lmpl::key_code::K, 2, 1},
        {lm::pi, lmpl::key_code::U, 2, 2},
        {lm::pi, lmpl::key_code::O, 2, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::J, 0, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::L, 0, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::I, 0, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::K, 0, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::U, 0, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::O, 0, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::J, 1, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::L, 1, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::I, 1, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::K, 1, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::U, 1, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::O, 1, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::J, 2, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::L, 2, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::I, 2, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::K, 2, 1},
        {lm::pi + lm::pi / 2, lmpl::key_code::U, 2, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::O, 2, 1},
      }));

    model.select_box(cubes[initial_selection]);

    model.camera.rotate({0.f, camera_rotation});

    lmtk::input_state input_state{};

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = key_code,
      });

    float pi = lm::pi;
    Eigen::Vector3f camera_dir =
      model.camera.rotation * Eigen::Vector3f::UnitZ();
    CAPTURE(
      camera_rotation / pi,
      key_code,
      initial_selection,
      expected_selection,
      camera_dir.transpose());

    REQUIRE(model.selected_box == cubes[expected_selection]);
}

TEST_CASE("Move selection snaps to world axes")
{
    entt::registry map;
    lmeditor::map_editor_model model{lmeditor::map_editor_model::init{
      .camera_init = lmeditor::orbital_camera_init{
        .fov = (float)M_PI / 3,
        .aspect = (float)16.f / 9.f,
        .near_clip = 0.1f,
        .far_clip = 1000.f,
        .position = Eigen::Vector3f{0.f, 10.f, -25.f},
        .target = Eigen::Vector3f{0.f, 0.f, 0.f},
      }}};

    // In view space the second cube is "higher", but it shouldn't
    // be selected with the "up" button while the camera's pitch is
    // closer to horizontal.
    std::array cubes{
      model.add_cube(map, {2.f, 2.f, 2.f}, 1.f),
      model.add_cube(map, {2.f, 0.f, 16.f}, 1.f),
    };

    model.select_box(cubes[1]);

    lmtk::input_state input_state{};

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = lmpl::key_code::I,
      });

    REQUIRE(model.selected_box == cubes[0]);
}

TEST_CASE_METHOD(model_test_case, "Move selection minimum distance")
{
    std::array cubes{
      model.add_cube(map, {-2.f, 0.f, 0.f}, 1.f),
      model.add_cube(map, {2.f, 0.01f, 0.f}, 1.f),
      model.add_cube(map, {0.f, -2.f, 0.f}, 1.f),
    };

    model.select_box(cubes[0]);

    model.move_selection(map, {0.f, -1.f, 0.f});

    REQUIRE(model.selected_box == cubes[2]);
}

TEST_CASE_METHOD(model_test_case, "Move selection with no selection")
{
    std::array cubes{
      model.add_cube(map, {0.f, 0.f, 0.f}, 1.f),
      model.add_cube(map, {2.f, 2.f, 2.f}, 1.f),
      model.add_cube(map, {4.f, 4.f, 4.f}, 1.f),
    };

    auto [camera_rotation, key_code, expected_selection] =
      GENERATE(table<float, lmpl::key_code, int>({
        {0.f, lmpl::key_code::J, 0},
        {0.f, lmpl::key_code::L, 2},
        {0.f, lmpl::key_code::I, 2},
        {0.f, lmpl::key_code::K, 0},
        {0.f, lmpl::key_code::U, 0},
        {0.f, lmpl::key_code::O, 2},
        {lm::pi / 2, lmpl::key_code::J, 2},
        {lm::pi / 2, lmpl::key_code::L, 0},
        {lm::pi / 2, lmpl::key_code::I, 2},
        {lm::pi / 2, lmpl::key_code::K, 0},
        {lm::pi / 2, lmpl::key_code::U, 0},
        {lm::pi / 2, lmpl::key_code::O, 2},
        {lm::pi, lmpl::key_code::J, 2},
        {lm::pi, lmpl::key_code::L, 0},
        {lm::pi, lmpl::key_code::I, 2},
        {lm::pi, lmpl::key_code::K, 0},
        {lm::pi, lmpl::key_code::U, 2},
        {lm::pi, lmpl::key_code::O, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::J, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::L, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::I, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::K, 0},
        {lm::pi + lm::pi / 2, lmpl::key_code::U, 2},
        {lm::pi + lm::pi / 2, lmpl::key_code::O, 0},
      }));

    model.camera.rotate({0.f, camera_rotation});

    lmtk::input_state input_state{};

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = key_code,
      });

    float pi = lm::pi;

    CAPTURE(camera_rotation / pi, model.camera.dir().transpose());

    REQUIRE(model.selected_box == cubes[expected_selection]);
}

TEST_CASE_METHOD(model_test_case, "Lone entity selection")
{
    lmtk::input_state input_state{};

    auto cube_id = model.add_cube(map, {0.f, 0.f, 0.f}, 1.f);

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = GENERATE(
          lmpl::key_code::J,
          lmpl::key_code::L,
          lmpl::key_code::I,
          lmpl::key_code::K,
          lmpl::key_code::U,
          lmpl::key_code::O),
      });

    REQUIRE(model.selected_box == cube_id);
}
