#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <lmlib/geometry.h>
#include <lmlib/math_constants.h>

using namespace lm;

TEST_CASE("Perspective transform sanity")
{
    auto point = Eigen::Vector4f{-5.f, 0.f, 5.f, 1.f};

    auto frustum = lm::frustum{lm::pi / 2, 1.f, 1.f, 1000.f};

    auto perspective_matrix = frustum.perspective_transform();

    auto transformed = perspective_matrix * point;

    REQUIRE(transformed[0] / transformed[3] == Approx(-1.f));
}
