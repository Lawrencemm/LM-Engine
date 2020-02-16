#pragma once
#include "geometry.h"
#include "lmlib/rotation_matrix.h"

namespace lm
{
struct camera_init
{
    float fov, aspect, near_clip, far_clip;
    Eigen::Vector3f position{0.f, 0.f, 0.f};
    Eigen::Quaternionf rotation{1.f, 0.f, 0.f, 0.f};
};

class camera
{
  public:
    camera() = default;
    camera(const camera_init &init)
        : fov{init.fov},
          aspect{init.aspect},
          near_clip{init.near_clip},
          far_clip{init.far_clip},
          pos{init.position},
          rotation{init.rotation}
    {
    }
    Eigen::Matrix4f projection_matrix() const;
    Eigen::Matrix4f inverse_transform_matrix() const;
    Eigen::Matrix4f view_matrix() const;
    std::pair<Eigen::Vector3f, Eigen::Vector3f>
      ray_from_point(float x, float y);
    Eigen::Vector3f dir() const;

    Eigen::Vector4f get_ndc(const Eigen::Vector3f &point) const;

    float fov, aspect, near_clip, far_clip;
    Eigen::Vector3f pos;
    Eigen::Quaternionf rotation;
    Eigen::Matrix4f transform_matrix() const;
};
} // namespace lm