#include <lmng/meta/any_component.h>
#include <fmt/format.h>

namespace lmng
{
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

char const *get_data_name(entt::meta_data const &data)
{
    return data.prop("name"_hs.value()).value().cast<char const *>();
}

char const *get_type_name(entt::meta_type const &type)
{
    return type.prop("name"_hs.value()).value().cast<char const *>();
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

void assign_or_replace_on_entity(
  const entt::meta_any &component,
  entt::registry &registry,
  entt::entity entity)
{
    component.type()
      .func("assign_or_replace_on_entity"_hs)
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

meta_type_map create_meta_type_map()
{
    meta_type_map map;

    entt::resolve([&](entt::meta_type const &meta_type) {
      map[meta_type.id()] = meta_type;
    });

    return std::move(map);
}

any_component::any_component(
  const entt::registry &registry,
  entt::entity entity,
  const entt::meta_type &type)
  : any{lmng::get_component_any(registry, entity, type)}
{
}

char const *any_component::name() const
{
    return any.type()
             .prop(entt::hashed_string{"name"}.value())
             .value()
             .cast<char const *>();
}

std::string any_component::get(
  const entt::meta_data &data,
  const entt::registry &registry) const
{
    return get_data(any, data, registry);
}

any_component &any_component::set(
  const entt::meta_data &data,
  const std::string &from,
  const entt::registry &registry)
{
    set_data(any, data, from, registry);
    return *this;
}

any_component &
  any_component::assign(entt::registry &registry, entt::entity entity)
{
    assign_to_entity(any, registry, entity);
    return *this;
}

any_component &
  any_component::replace(entt::registry &registry, entt::entity entity)
{
    replace_on_entity(any, registry, entity);
    return *this;
}
}