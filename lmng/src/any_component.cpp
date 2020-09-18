#include <cassert>
#include <fmt/format.h>
#include <lmng/meta/any_component.h>
#include <spdlog/spdlog.h>

namespace lmng
{
entt::meta_any get_component_any(
  entt::registry const &registry,
  entt::entity entity,
  entt::meta_type const &type)
{
    auto get_func = type.func("get_from_entity"_hs);

    assert(
      (fmt::format(
         "Failed resolving meta getter function for component {}",
         get_type_name(type)),
       get_func));

    auto component_any = get_func.invoke(
      {},
      std::reference_wrapper(const_cast<entt::registry &>(registry)),
      entity);

    assert(
      (fmt::format(
         "Failed invoking meta getter for component ", get_type_name(type)),
       component_any));

    return component_any;
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
    auto func = component.type().func("assign_to_entity"_hs);

    assert(
      (fmt::format(
         "Failed to lookup assign_to_entity meta function for component {}",
         get_type_name(component.type())),
       func));

    auto result =
      func.invoke({}, component, std::reference_wrapper(registry), entity);

    assert(
      (fmt::format(
         "Failed to invoke assign_to_entity meta function for component {}",
         get_type_name(component.type())),
       result));
}

void replace_on_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity)
{
    auto func = component.type().func("replace_on_entity"_hs);

    assert(
      (fmt::format(
         "Failed lookup of replace_on_entity function on component {}",
         get_type_name(component.type())),
       func));

    auto result =
      func.invoke({}, component, std::reference_wrapper(registry), entity);

    assert(
      (fmt::format(
         "Failed invocation of replace_on_entity function for component {}",
         get_type_name(component.type())),
       result));
}

void emplace_or_replace_on_entity(
  const entt::meta_any &component,
  entt::registry &registry,
  entt::entity entity)
{
    component.type()
      .func("emplace_or_replace_on_entity"_hs)
      .invoke({}, component, std::reference_wrapper(registry), entity);
}

void remove_from_entity(
  entt::meta_type const &component_type,
  entt::registry &registry,
  entt::entity entity)
{
    component_type.func("remove_from_entity"_hs)
      .invoke({}, std::reference_wrapper(registry), entity);
}

std::string get_data(
  entt::meta_any const &component,
  entt::meta_data const &data,
  entt::registry const &context)
{
    auto as_string_name = fmt::format("get_{}_as_str", get_data_name(data));
    entt::meta_func as_string_func =
      component.type().func(entt::hashed_string{as_string_name.c_str()});

    const entt::meta_any &data_any = as_string_func.invoke(
      {},
      reinterpret_cast<char const *>(component.data()),
      std::reference_wrapper(const_cast<entt::registry &>(context)));

    assert(
      (fmt::format(
         "Invocation of meta getter function failed for "
         "{} on {}",
         get_data_name(data),
         get_type_name(component.type())),
       data_any));

    return data_any.cast<std::string>();
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

    SPDLOG_INFO(
      "{} -> {} ({})",
      string,
      get_data_name(data),
      get_type_name(component.type()));

    auto res = from_string.invoke(
      {},
      reinterpret_cast<char *>(component.data()),
      std::reference_wrapper(const_cast<std::string &>(string)),
      std::reference_wrapper(const_cast<entt::registry &>(context)));

    assert(
      (fmt::format(
         "Failed invoking meta data setter for data {} on component {}",
         get_data_name(data),
         get_type_name(component.type())),
       res));
}

meta_type_map create_meta_type_map()
{
    meta_type_map map;

    for (auto const &meta_type : entt::resolve())
    {
        map[meta_type.id()] = meta_type;
    };

    return std::move(map);
}

any_component::any_component(
  entt::registry const &registry,
  entt::entity entity,
  entt::meta_type const &type)
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
} // namespace lmng
