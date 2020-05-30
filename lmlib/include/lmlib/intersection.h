#pragma once

#include <Eigen/Eigen>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(
  Eigen::Vector2f,
  float,
  boost::geometry::cs::cartesian,
  x(),
  y());

namespace lm
{
using box2f = boost::geometry::model::box<Eigen::Vector2f>;
using segment2f = boost::geometry::model::segment<Eigen::Vector2f>;

box2f get_box(Eigen::Vector2f extents);

std::pair<segment2f, float>
  closest_edge(Eigen::Vector2f point, Eigen::Vector2f box_extents);

float distance(Eigen::Vector2f point, Eigen::Vector2f box_extents);
} // namespace lm
