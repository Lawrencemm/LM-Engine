#pragma once

#include "../hierarchy.h"
#include "../logging.h"
#include "../name.h"
#include "any_component.h"
#include "signal.h"
#include <Eigen/Eigen>
#include <entt/entt.hpp>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

namespace lmng
{
template <typename component_type>
void assign_to_entity(
  component_type const &component,
  entt::registry &registry,
  entt::entity entity)
{
    registry.emplace<component_type>(entity, component);
}

template <typename component_type>
void replace_on_entity(
  component_type const &component,
  entt::registry &registry,
  entt::entity entity)
{
    registry.replace<component_type>(entity, component);
}

template <typename component_type>
void emplace_or_replace_on_entity(
  component_type const &component,
  entt::registry &registry,
  entt::entity entity)
{
    registry.emplace_or_replace<component_type>(entity, component);
}

template <typename component_type>
component_type
  get_from_entity(entt::registry const &registry, entt::entity entity)
{
    if constexpr (!std::is_empty_v<component_type>)
    {
        return registry.get<component_type>(entity);
    }
    else
    {
        return component_type{};
    }
}

template <typename component_type>
void remove_from_entity(entt::registry &registry, entt::entity entity)
{
    return registry.remove<component_type>(entity);
}

template <typename component_type> auto construct() { return component_type{}; }

template <typename MemPtr> struct get_owner_type;

template <typename Var, typename Class> struct get_owner_type<Var(Class::*)>
{
    using type = Class;
};

template <typename data_type>
void stream_data_out(
  std::ostream &stream,
  data_type const &data,
  entt::registry const &context)
{
    stream << data;
}

template <typename data_type>
void stream_data_in(
  std::istream &stream,
  data_type &data,
  entt::registry const &context)
{
    stream >> data;
}

void stream_data_out(
  std::ostream &stream,
  std::array<float, 3> const &data,
  entt::registry const &context);

void stream_data_in(
  std::istream &stream,
  std::array<float, 3> &data,
  entt::registry const &context);

template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Vector3f const &data,
  entt::registry const &context);

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Vector3f &data,
  entt::registry const &context);

template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Quaternionf const &data,
  entt::registry const &context);

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Quaternionf &data,
  entt::registry const &context);

void stream_data_out(
  std::ostream &stream,
  entt::entity const &data,
  entt::registry const &context);

void stream_data_in(
  std::istream &stream,
  entt::entity &data,
  entt::registry const &context);

template <auto member_ptr>
void setter(
  char *component,
  std::string const &str,
  entt::registry const &context)
{
    using component_type = typename get_owner_type<decltype(member_ptr)>::type;
    auto typed_component = reinterpret_cast<component_type *>(component);
    std::istringstream stream{str};
    stream_data_in(stream, typed_component->*member_ptr, context);
}

template <auto member_ptr>
std::string getter(char const *component, entt::registry const &context)
{
    using component_type = typename get_owner_type<decltype(member_ptr)>::type;
    auto typed_component = reinterpret_cast<component_type const *>(component);
    std::ostringstream stream{};
    stream_data_out(stream, typed_component->*member_ptr, context);
    return stream.str();
}

template <typename component_type>
std::string output_data_for_log(entt::registry &registry, entt::entity entity)
{
    if constexpr (std::is_empty_v<component_type>)
    {
        return "";
    }
    else
    {
        component_type &component = registry.get<component_type>(entity);
        entt::meta_any component_any{
          std::reference_wrapper<component_type>(component)};

        return output_data_for_log(component_any, registry);
    }
}

template <typename component_type>
void log_component_signal(
  entt::registry &registry,
  entt::entity entity,
  char const *event_indicator)
{
    SPDLOG_INFO(
      "{} {} {} ({}) {{{}}}",
      get_type_name(entt::resolve<component_type>()),
      event_indicator,
      get_name(registry, entity),
      std::to_string(to_integral(entity)),
      output_data_for_log<component_type>(registry, entity));
}

template <typename component_type>
void log_assign(entt::registry &registry, entt::entity entity)
{
    log_component_signal<component_type>(registry, entity, "->");
}

template <typename component_type>
void log_replace(entt::registry &registry, entt::entity entity)
{
    log_component_signal<component_type>(registry, entity, "/->");
}

template <typename component_type>
void log_destroy(entt::registry &registry, entt::entity entity)
{
    log_component_signal<component_type>(registry, entity, "X");
}

template <typename component_type>
void connect_logging(entt::registry &registry)
{
    registry.on_construct<component_type>()
      .template connect<&log_assign<component_type>>();
    registry.on_update<component_type>()
      .template connect<log_replace<component_type>>();
    registry.on_destroy<component_type>()
      .template connect<log_destroy<component_type>>();
}

template <typename component_type>
void copy_pool(entt::registry &dest, entt::registry const &src)
{
    auto p_entities = src.data<component_type>();

    if constexpr (!std::is_empty_v<component_type>)
    {
        auto p_data = src.raw<component_type>();

        dest.insert<component_type>(
          p_entities,
          p_entities + src.size<component_type>(),
          p_data,
          p_data + src.size());
    }
    else
    {
        dest.insert<component_type>(
          p_entities, p_entities + src.size<component_type>(), {});
    }
}

template <typename component_type>
void dispatch_construct(
  any_component_listener &listener,
  entt::registry &registry,
  entt::entity entity)
{
    listener.on_construct_any(
      registry, entity, entt::resolve<component_type>());
}

template <typename component_type>
void dispatch_replace(
  any_component_listener &listener,
  entt::registry &registry,
  entt::entity entity)
{
    listener.on_update_any(registry, entity, entt::resolve<component_type>());
}

template <typename component_type>
void dispatch_destroy(
  any_component_listener &listener,
  entt::registry &registry,
  entt::entity entity)
{
    listener.on_destroy_any(registry, entity, entt::resolve<component_type>());
}

template <typename component_type>
void connect_construct(
  entt::registry &registry,
  any_component_listener &listener)
{
    registry.on_construct<component_type>()
      .template connect<&dispatch_construct<component_type>>(listener);
}

template <typename component_type>
void connect_replace(entt::registry &registry, any_component_listener &listener)
{
    registry.on_update<component_type>()
      .template connect<&dispatch_construct<component_type>>(listener);
}

template <typename component_type>
void connect_destroy(entt::registry &registry, any_component_listener &listener)
{
    registry.on_destroy<component_type>()
      .template connect<&dispatch_destroy<component_type>>(listener);
}

template <typename component_type>
void disconnect_construct(
  entt::registry &registry,
  any_component_listener &listener)
{
    registry.on_construct<component_type>()
      .template disconnect<&dispatch_construct<component_type>>(listener);
}

template <typename component_type>
void disconnect_replace(
  entt::registry &registry,
  any_component_listener &listener)
{
    registry.on_update<component_type>()
      .template disconnect<&dispatch_construct<component_type>>(listener);
}

template <typename component_type>
void disconnect_destroy(
  entt::registry &registry,
  any_component_listener &listener)
{
    registry.on_destroy<component_type>()
      .template disconnect<&dispatch_destroy<component_type>>(listener);
}
} // namespace lmng

#define REFLECT_MEMBER(type, member, name)                                     \
    data<&type::member>(name##_hs)                                             \
      .prop("name"_hs.value(), name)                                           \
      .func<&lmng::setter<&type::member>>(                                     \
        entt::hashed_string{"set_" name "_from_str"})                          \
      .func<&lmng::getter<&type::member>>(                                     \
        entt::hashed_string{"get_" name "_as_str"})

#define REFLECT_TYPE(_type, name)                                              \
    entt::meta<_type>()                                                        \
      .type(entt::hashed_string{name}.value())                                 \
      .prop("name"_hs.value(), name)                                           \
      .prop(entt::hashed_string{"is_component"})                               \
      .ctor<&lmng::construct<_type>>()                                         \
      .func<&lmng::assign_to_entity<_type>>("assign_to_entity"_hs)             \
      .func<&lmng::replace_on_entity<_type>>("replace_on_entity"_hs)           \
      .func<&lmng::emplace_or_replace_on_entity<_type>>(                       \
        "emplace_or_replace_on_entity"_hs)                                     \
      .func<&lmng::get_from_entity<_type>>("get_from_entity"_hs)               \
      .func<&lmng::remove_from_entity<_type>>("remove_from_entity"_hs)         \
      .func<&lmng::connect_logging<_type>>("connect_logging"_hs)               \
      .func<&lmng::connect_construct<_type>>("connect_construct"_hs)           \
      .func<&lmng::connect_replace<_type>>("connect_replace"_hs)               \
      .func<&lmng::connect_destroy<_type>>("connect_destroy"_hs)               \
      .func<&lmng::disconnect_construct<_type>>("disconnect_construct"_hs)     \
      .func<&lmng::disconnect_replace<_type>>("disconnect_replace"_hs)         \
      .func<&lmng::disconnect_destroy<_type>>("disconnect_destroy"_hs)         \
      .func<&lmng::copy_pool<_type>>("copy_pool"_hs)
