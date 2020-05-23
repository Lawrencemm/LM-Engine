#include <entt/entt.hpp>

#include <fmt/format.h>
#include <lmng/camera.h>
#include <lmng/kinematic.h>
#include <lmng/name.h>
#include <lmng/physics.h>
#include <lmng/reflection.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>

namespace lmng
{
char const *get_data_name(entt::meta_data const &data)
{
    return data.prop("name"_hs.value()).value().cast<char const *>();
}
char const *get_type_name(entt::meta_type const &type)
{
    return type.prop("name"_hs.value()).value().cast<char const *>();
}

entt::meta_any get_component_any(
  entt::registry const &registry,
  entt::entity entity,
  entt::meta_type const &type)
{
    auto get_func = type.func("get_from_entity"_hs);
    if (!get_func)
        throw std::runtime_error{"entt meta error."};
    return get_func.invoke({}, &registry, entity);
}

void assign_to_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity)
{
    component.type()
      .func("assign_to_entity"_hs)
      .invoke({}, component.data(), &registry, entity);
}

void replace_on_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity)
{
    component.type()
      .func("replace_on_entity"_hs)
      .invoke({}, component.data(), &registry, entity);
}

void remove_from_entity(
  entt::meta_type const &component_type,
  entt::registry &registry,
  entt::entity entity)
{
    component_type.func("remove_from_entity"_hs).invoke({}, &registry, entity);
}

std::string get_data(
  entt::meta_any const &component,
  entt::meta_data const &data,
  entt::registry const &context)
{
    auto as_string_name = fmt::format("get_{}_as_str", get_data_name(data));
    entt::meta_func to_string =
      component.type().func(entt::hashed_string{as_string_name.c_str()});
    return to_string.invoke({}, component.data(), &context).cast<std::string>();
}

void set_data(
  entt::meta_any &component,
  entt::meta_data const &data,
  std::string const &string,
  entt::registry const &context)
{
    auto from_string_name = fmt::format("set_{}_from_str", get_data_name(data));
    entt::meta_func from_string =
      component.type().func(entt::hashed_string{from_string_name.c_str()});
    from_string.invoke({}, component.data(), &string, &context);
}

void clone(
  entt::registry const &from,
  entt::registry &to,
  entt::meta_type const &type)
{
    auto meta_func = type.func("clone"_hs);
    meta_func.invoke({}, &from, &to);
}

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
    stream << context->get<name>(data).string;
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

meta_type_map create_meta_type_map()
{
    meta_type_map map;

    entt::resolve([&](entt::meta_type const &meta_type) {
        map[meta_type.id()] = meta_type;
    });

    return std::move(map);
}

void set_meta_context(entt::meta_ctx const &ctx) { entt::meta_ctx::bind(ctx); }

void reflect_types()
{
    REFLECT_TYPE(box_render, "Box Render")
      .REFLECT_MEMBER(box_render, extents, "Extents")
      .REFLECT_MEMBER(box_render, colour, "Colour");

    REFLECT_TYPE(transform, "Transform")
      .REFLECT_MEMBER(transform, position, "Position")
      .REFLECT_MEMBER(transform, rotation, "Rotation");

    REFLECT_TYPE(transform_parent, "Transform Parent")
      .REFLECT_MEMBER(transform_parent, entity, "Entity");

    REFLECT_TYPE(rigid_body, "Rigid Body")
      .REFLECT_MEMBER(rigid_body, mass, "Mass")
      .REFLECT_MEMBER(rigid_body, restitution, "Restitution")
      .REFLECT_MEMBER(rigid_body, friction, "Friction");

    REFLECT_TYPE(character_controller, "Character Controller");

    REFLECT_TYPE(box_collider, "Box Collider")
      .REFLECT_MEMBER(box_collider, extents, "Extents");

    REFLECT_TYPE(camera, "Camera")
      .REFLECT_MEMBER(camera, field_of_view, "Field of view")
      .REFLECT_MEMBER(camera, near_clip, "Near clip")
      .REFLECT_MEMBER(camera, far_clip, "Far clip")
      .REFLECT_MEMBER(camera, active, "Active");
}
} // namespace lmng
