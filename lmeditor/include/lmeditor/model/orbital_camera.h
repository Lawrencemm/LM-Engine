#pragma once

#include <lmlib/camera.h>

namespace lmeditor
{
struct orbital_camera_init;
class orbital_camera : public lm::camera
{
  public:
    orbital_camera() = default;
    orbital_camera(const orbital_camera_init &init);

    orbital_camera &rotate(Eigen::Vector2f const &lat_lon);
    orbital_camera &move(const float distance);
    orbital_camera &move_to_target(const Eigen::Vector3f &target);

    Eigen::Vector3f calculate_target_pos() const;
    void calculate_rotation();
    float get_distance();

    Eigen::Vector3f spherical_coordinates;
};

struct orbital_camera_init
{
    float fov, aspect, near_clip, far_clip;
    Eigen::Vector3f position, target;
};
} // namespace lmeditor