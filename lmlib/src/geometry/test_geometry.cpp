#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <lmlib/geometry.h>
#include <lmlib/graphics/camera.h>
#include <lmlib/math/math.h>
#include <lmlib/util/enumerate.h>

#include "lmlib/math/test_utils.h"

using namespace lm;

TEST_CASE("Perspective transform sanity")
{
    auto point = Eigen::Vector4f{-5.f, 0.f, 5.f, 1.f};

    auto frustum = lm::frustum{(float)M_PI / 2, 1.f, 1.f, 1000.f};

    auto perspective_matrix = frustum.perspective_transform();

    auto transformed = perspective_matrix * point;

    REQUIRE(transformed[0] / transformed[3] == Approx(-1.f));
}

TEST_CASE("Perspective transform dunno")
{
    auto z = 1.f;
    auto points = {Eigen::Vector3f(0.0f, -0.5f, z),
                   Eigen::Vector3f{0.5f, 0.5f, z},
                   Eigen::Vector3f{-0.5f, 0.5f, z}};

    std::array<std::pair<Eigen::Vector3f, Eigen::Vector3f>, 3> map;

    auto frustum = lm::frustum{0.75f, 16.f / 9.f, 0.1f, 1000.f};

    auto perspective_matrix = frustum.perspective_transform();

    for (auto [i, vertex] : enumerate(points))
    {
        Eigen::Vector4f w_point;
        w_point << vertex, 1.f;

        auto clip_point = perspective_matrix * w_point;

        auto to_point = Eigen::Vector3f{clip_point[0] / clip_point[3],
                                        clip_point[1] / clip_point[3],
                                        clip_point[2] / clip_point[3]};
    }
}

TEST_CASE("Camera ray from point")
{
    using namespace fmt::literals;
    auto z = 10.f;
    auto scale = 2.5f;
    auto screen_size_x = 800, screen_size_y = 600;
    auto fov = 0.75f, aspect = (float)screen_size_x / screen_size_y,
         near = 0.1f, far = 1000.f;
    auto camera = lm::camera{fov, aspect, near, far};

    auto perspective_matrix = camera.projection_matrix();
    auto inverse_perspective = perspective_matrix.inverse();

    auto points = {Eigen::Vector3f{scale, scale, z},
                   Eigen::Vector3f{scale, -scale, z},
                   Eigen::Vector3f{-scale, -scale, z},
                   Eigen::Vector3f{-scale, scale, z}};

    for (auto point : points)
    {
        auto point4 = Eigen::Vector4f{};
        point4 << point, 1.f;
        auto clip = perspective_matrix * point4;
        auto ndc = clip.head(3) / clip[3];

        auto ray = camera.ray_from_point(ndc[0], ndc[1]);

        CHECK(ray.first.norm() > 0.f);
        CHECK(ray.second.norm() > 0.f);

        auto line = Eigen::ParametrizedLine<float, 3>{
          ray.first, (ray.second - ray.first).normalized()};

        CHECK(line.origin().norm() > 0.f);
        CHECK(line.direction().norm() > 0.f);

        // Eigen-equal
        //    CHECK(ray.first, line.origin());
        //    EXPECT_EIGEN_EQUAL(ray.second,
        //                       line.origin() +
        //                           line.direction() * (ray.second -
        //                           ray.first).norm());

        CHECK(line.distance(point) <= 1.f);
    }
}
