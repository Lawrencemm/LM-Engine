#pragma once

#include <Eigen/Eigen>
#include <entt/entt.hpp>

namespace lmng
{
struct meta_component
{
    std::vector<entt::meta_type> component_types;
};

entt::meta_any get_component_any(
  entt::registry &registry,
  entt::entity entity,
  entt::meta_type const &type);

std::string get_data(
  entt::meta_any const &component,
  entt::meta_data const &data,
  entt::registry const &context);

void set_data(
  entt::meta_any &component,
  entt::meta_data const &data,
  std::string const &string,
  entt::registry const &context);

void assign_to_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity);

void replace_on_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity);

void remove_from_entity(
  entt::meta_type const &component_type,
  entt::registry &registry,
  entt::entity entity);

void clone(
  entt::registry const &from,
  entt::registry &to,
  entt::meta_type const &type);

class any_component
{
  public:
    any_component(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &type)
        : any{lmng::get_component_any(registry, entity, type)}
    {
    }

    char const *name() const
    {
        return any.type()
          .prop(entt::hashed_string{"name"}.value())
          .value()
          .cast<char const *>();
    }

    std::string
      get(entt::meta_data const &data, entt::registry const &registry) const
    {
        return get_data(any, data, registry);
    }

    any_component &set(
      entt::meta_data const &data,
      std::string const &from,
      entt::registry const &registry)
    {
        set_data(any, data, from, registry);
        return *this;
    }

    any_component &assign(entt::registry &registry, entt::entity entity)
    {
        assign_to_entity(any, registry, entity);
        return *this;
    }

    any_component &replace(entt::registry &registry, entt::entity entity)
    {
        replace_on_entity(any, registry, entity);
        return *this;
    }

    entt::meta_any any;
};

/// Supply a function to be called with every reflected component on the entity.
template <typename function_type>
void reflect_components(
  entt::registry &registry,
  entt::entity entity,
  function_type const &function)
{
    auto &reflection_component = registry.get<struct meta_component>(entity);
    for (auto &type : reflection_component.component_types)
    {
        function(any_component{registry, entity, type});
    }
}

template <typename component_type>
void assign_to_entity(
  void const *component,
  entt::registry *registry,
  entt::entity entity)
{
    registry->assign<component_type>(
      entity, *static_cast<component_type const *>(component));
}

template <typename component_type>
void replace_on_entity(
  void const *component,
  entt::registry *registry,
  entt::entity entity)
{
    registry->replace<component_type>(
      entity, *static_cast<component_type const *>(component));
}

template <typename component_type>
component_type get_from_entity(entt::registry *registry, entt::entity entity)
{
    return registry->get<component_type>(entity);
}

template <typename component_type>
void remove_from_entity(entt::registry *registry, entt::entity entity)
{
    return registry->remove<component_type>(entity);
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
  entt::registry const *)
{
    stream << data;
}

template <typename data_type>
void stream_data_in(
  std::istream &stream,
  data_type &data,
  entt::registry const *)
{
    stream >> data;
}

void stream_data_out(
  std::ostream &stream,
  std::array<float, 3> const &data,
  entt::registry const *);

void stream_data_in(
  std::istream &stream,
  std::array<float, 3> &data,
  entt::registry const *);

template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Vector3f const &data,
  entt::registry const *);

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Vector3f &data,
  entt::registry const *);

template <>
void stream_data_out(
  std::ostream &stream,
  Eigen::Quaternionf const &data,
  entt::registry const *);

template <>
void stream_data_in(
  std::istream &stream,
  Eigen::Quaternionf &data,
  entt::registry const *);

void stream_data_out(
  std::ostream &stream,
  entt::entity const &data,
  entt::registry const *);

void stream_data_in(
  std::istream &stream,
  entt::entity &data,
  entt::registry const *);

template <auto member_ptr>
void setter(
  void *component,
  std::string const *str,
  entt::registry const *context)
{
    using component_type = typename get_owner_type<decltype(member_ptr)>::type;
    auto typed_component = static_cast<component_type *>(component);
    std::istringstream stream{*str};
    stream_data_in(stream, typed_component->*member_ptr, context);
}

template <auto member_ptr>
std::string getter(void const *component, entt::registry const *context)
{
    using component_type = typename get_owner_type<decltype(member_ptr)>::type;
    auto typed_component = static_cast<component_type const *>(component);
    std::ostringstream stream{};
    stream_data_out(stream, typed_component->*member_ptr, context);
    return stream.str();
}

template <typename component_type>
void clone(entt::registry const *from, entt::registry *to)
{
    from->view<component_type const>().each(
      [&](auto entity, auto const &component) {
          to->assign<component_type>(entity, component);
      });
}

char const *get_type_name(entt::meta_type const &type);
char const *get_data_name(entt::meta_data const &data);

void set_meta_context(entt::meta_ctx const &ctx);
void reflect_types();
} // namespace lmng

ENTT_NAMED_TYPE(uint32_t);
ENTT_NAMED_TYPE(char const *);
ENTT_NAMED_TYPE(entt::registry *);
ENTT_NAMED_TYPE(entt::entity);
ENTT_NAMED_TYPE(void const *);
ENTT_NAMED_TYPE(void *);
ENTT_NAMED_TYPE(std::string);
ENTT_NAMED_TYPE(std::string const *);
ENTT_NAMED_TYPE(bool);
ENTT_NAMED_TYPE(entt::registry const *);
ENTT_NAMED_TYPE(lmng::meta_component);
ENTT_NAMED_TYPE(entt::hashed_string);

#define REFLECT_MEMBER(type, member, name)                                     \
    data<&type::member>(name##_hs)                                             \
      .prop("name"_hs.value(), name)                                           \
      .func<&lmng::setter<&type::member>>(                                     \
        entt::hashed_string{"set_" name "_from_str"})                          \
      .func<&lmng::getter<&type::member>>(                                     \
        entt::hashed_string{"get_" name "_as_str"})

#define REFLECT_TYPE(_type, name)                                              \
    entt::meta<_type>()                                                        \
      .type()                                                                  \
      .prop("name"_hs.value(), name)                                           \
      .prop(entt::hashed_string{"is_component"})                               \
      .ctor<&lmng::construct<_type>>()                                         \
      .func<&lmng::assign_to_entity<_type>>("assign_to_entity"_hs)             \
      .func<&lmng::replace_on_entity<_type>>("replace_on_entity"_hs)           \
      .func<&lmng::get_from_entity<_type>>("get_from_entity"_hs)               \
      .func<&lmng::remove_from_entity<_type>>("remove_from_entity"_hs)         \
      .func<&lmng::clone<_type>>("clone"_hs)
