#include <entt/entt.hpp>
#include <fmt/format.h>
#include <lmng/kinematic.h>
#include <lmng/meta/reflect_component.h>
#include <lmng/name.h>

namespace lmng
{
template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Vector3f const &data,
  entt::registry const *)
{
    stream << fmt::format("{} {} {}", data[0], data[1], data[2]);
}

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Vector3f &data,
  entt::registry const *)
{
    stream >> data[0] >> data[1] >> data[2];
}

template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Quaternionf const &data,
  entt::registry const *)
{
    stream << fmt::format(
      "{} {} {} {}", data.x(), data.y(), data.z(), data.w());
}

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Quaternionf &data,
  entt::registry const *)
{
    stream >> data.x() >> data.y() >> data.z() >> data.w();
}

void stream_data_out(
  std::ostream &stream,
  std::array<float, 3> const &data,
  entt::registry const *)
{
    stream << fmt::format("{} {} {}", data[0], data[1], data[2]);
}

void stream_data_in(
  std::istream &stream,
  std::array<float, 3> &data,
  entt::registry const *)
{
    for (auto &datum : data)
        stream >> datum;
}

void stream_data_out(
  std::ostream &stream,
  entt::entity const &data,
  entt::registry const *context)
{
    stream << get_name(*context, data);
}

void stream_data_in(
  std::istream &stream,
  entt::entity &data,
  entt::registry const *context)
{
    std::string entity_name(std::istreambuf_iterator<char>(stream), {});

    data = find_entity(*context, entity_name);
    if (data == entt::null)
        throw std::runtime_error{"Entity named " + entity_name + " not found."};
}
} // namespace lmng
