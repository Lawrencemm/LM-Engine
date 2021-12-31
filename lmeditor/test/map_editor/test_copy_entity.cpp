#include "map_editor_controller_test_case.h"
#include <catch2/catch.hpp>

TEST_CASE_METHOD(map_editor_controller_test_case, "Copy entity")
{
    auto cube = controller.add_cube(map, {0.f, 0.f, 0.f}, 1.f);
    controller.select(cube);

    controller.copy_entity(map, Eigen::Vector3f::UnitX());
}
