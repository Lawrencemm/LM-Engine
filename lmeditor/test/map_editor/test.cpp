#include "model_test_case.h"

#include <catch2/catch.hpp>

TEST_CASE_METHOD(model_test_case, "Add cube position")
{
    Eigen::Vector3f cube_pos{2.f, 0.f, 0.f};
    auto cube_id = model.add_cube(map, cube_pos, 1.f);

    REQUIRE(map.get<lmng::transform>(cube_id).position == cube_pos);
}

TEST_CASE_METHOD(model_test_case, "Deselect")
{
    auto cube = model.add_cube(map, {0.f, 0.f, 0.f}, 1.f);

    lmtk::input_state input_state{};

    model.select_box(cube);

    REQUIRE(model.selected_box == cube);

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = lmpl::key_code::N,
      });

    REQUIRE(model.selected_box != cube);
}

TEST_CASE_METHOD(model_test_case, "Add new fires entity added event")
{
    lmtk::input_state const &input_state{};
    bool entity_created_fired{false};

    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = lmpl::key_code::A,
      },
      lmeditor::map_editor_event_handler{}.set_on_created_entity(
        [&](lmeditor::map_editor_created_entity const &) {
            entity_created_fired = true;
        }),
      lmeditor::map_editor_model::state_change_handler());

    REQUIRE(entity_created_fired);
}

TEST_CASE_METHOD(
  model_test_case,
  "Destroy current fires destroying entity event")
{
    lmtk::input_state const &input_state{};
    bool destroying_entity_fired{false};
    auto box = model.add_cube(map, {0.f, 0.f, 0.f}, 1.f);

    model.select_box(box);
    model.handle(
      map,
      lmtk::key_down_event{
        .input_state = input_state,
        .key = lmpl::key_code::X,
      },
      lmeditor::map_editor_event_handler{}.set_on_destroying_entity(
        [&](lmeditor::map_editor_destroying_entity const &) {
            destroying_entity_fired = true;
        }),
      lmeditor::map_editor_model::state_change_handler());

    REQUIRE(destroying_entity_fired);
}
