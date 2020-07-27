#include "test_case.h"

#include <catch2/catch.hpp>

TEST_CASE_METHOD(visual_registry_controller_test_case, "Add cube position")
{
    Eigen::Vector3f cube_pos{2.f, 0.f, 0.f};
    auto cube_id = controller.add_cube(map, cube_pos, 1.f);

    REQUIRE(map.get<lmng::transform>(cube_id).position == cube_pos);
}

TEST_CASE_METHOD(visual_registry_controller_test_case, "Deselect")
{
    auto cube = controller.add_cube(map, {0.f, 0.f, 0.f}, 1.f);

    lmtk::input_state input_state{};

    controller.select(cube);

    REQUIRE(controller.get_selection() == cube);

    controller.handle(lmtk::key_down_event{
      .input_state = input_state,
      .key = lmpl::key_code::N,
    });

    REQUIRE(controller.get_selection() != cube);
}
