#include "orbital_camera.h"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>

#include <lmlib/eigen.h>

Eigen::Vector3f get_spherical_coordinates(Eigen::Vector3f const &vector)
{
    float distance = vector.norm();
    float longitude = std::atan2(vector[0], -vector[2]);
    float latitude = std::asin(vector[1] / distance);

    return {distance, latitude, longitude};
}

namespace lmeditor
{
orbital_camera::orbital_camera(const orbital_camera_init &init)
    : lm::camera{lm::camera_init{
        init.fov,
        init.aspect,
        init.near_clip,
        init.far_clip,
        init.position,
      }},
      spherical_coordinates{
        get_spherical_coordinates(init.position - init.target)}
{
    calculate_rotation();
}

orbital_camera &orbital_camera::rotate(Eigen::Vector2f const &lat_lon)
{
    Eigen::Vector3f target = calculate_target_pos();

    spherical_coordinates[1] += lat_lon[0];
    spherical_coordinates[2] += lat_lon[1];

    calculate_rotation();

    move_to_target(target);

    return *this;
}

void orbital_camera::calculate_rotation()
{
    rotation =
      Eigen::AngleAxisf{spherical_coordinates[2], Eigen::Vector3f::UnitY()} *
      Eigen::AngleAxisf{spherical_coordinates[1], Eigen::Vector3f::UnitX()};

    Eigen::Vector3f dir = rotation * Eigen::Vector3f::UnitZ();
}

Eigen::Vector3f orbital_camera::calculate_target_pos() const
{
    Eigen::Vector3f target =
      pos + rotation * Eigen::Vector3f{0.f, 0.f, spherical_coordinates[0]};
    return target;
}

orbital_camera &orbital_camera::move(const float distance)
{
    Eigen::Vector3f target = calculate_target_pos();
    spherical_coordinates[0] += distance;
    move_to_target(target);
    return *this;
}

orbital_camera &orbital_camera::move_to_target(const Eigen::Vector3f &target)
{
    pos =
      target - rotation * Eigen::Vector3f{0.f, 0.f, spherical_coordinates[0]};
    return *this;
}

float orbital_camera::get_distance() { return spherical_coordinates[0]; }
} // namespace lmeditor