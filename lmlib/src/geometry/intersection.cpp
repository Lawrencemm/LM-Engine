#include <Eigen/Eigen>
#include <boost/geometry.hpp>
#include <limits>
#include <lmlib/intersection.h>

namespace lm
{
box2f get_box(Eigen::Vector2f extents) { return box2f{-extents, extents}; }

float distance(Eigen::Vector2f point, Eigen::Vector2f box_extents)
{
    box2f box = get_box(box_extents);

    if (boost::geometry::within(point, box))
    {
        Eigen::Vector2f a{-box_extents[0], -box_extents[1]},
          b{-box_extents[0], box_extents[1]}, c{box_extents[0], box_extents[1]},
          d{box_extents[0], -box_extents[1]};

        std::initializer_list<segment2f> segments{
          {a, b}, {b, c}, {c, d}, {d, a}};

        float distance{std::numeric_limits<float>::max()};

        for (auto segment : segments)
        {
            distance = std::min(
              (float)boost::geometry::distance(segment, point), distance);
        }
        return -distance;
    }

    return boost::geometry::distance(point, box);
}

std::pair<segment2f, float>
  closest_edge(Eigen::Vector2f point, Eigen::Vector2f box_extents)
{
    Eigen::Vector2f a{-box_extents[0], -box_extents[1]},
      b{-box_extents[0], box_extents[1]}, c{box_extents[0], box_extents[1]},
      d{box_extents[0], -box_extents[1]};

    std::initializer_list<segment2f> segments{{a, b}, {b, c}, {c, d}, {d, a}};

    float min_distance{std::numeric_limits<float>::max()};
    segment2f closest;

    for (auto segment : segments)
    {
        double segment_distance = boost::geometry::distance(segment, point);
        if (segment_distance < min_distance)
        {
            min_distance = segment_distance;
            closest = segment;
        }
    }

    return {closest, min_distance};
}
} // namespace lm
