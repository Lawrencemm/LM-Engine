#pragma once

#include <entt/entt.hpp>

namespace lmng
{
entt::meta_any get_component_any(
  entt::registry const &registry,
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

void assign_or_replace_on_entity(
  entt::meta_any const &component,
  entt::registry &registry,
  entt::entity entity);

void remove_from_entity(
  entt::meta_type const &component_type,
  entt::registry &registry,
  entt::entity entity);

char const *get_type_name(entt::meta_type const &type);
char const *get_data_name(entt::meta_data const &data);

class any_component
{
  public:
    any_component(
      entt::registry const &registry,
      entt::entity entity,
      entt::meta_type const &type);

    char const *name() const;

    std::string
    get(entt::meta_data const &data, entt::registry const &registry) const;

    any_component &set(
      entt::meta_data const &data,
      std::string const &from,
      entt::registry const &registry);

    any_component &assign(entt::registry &registry, entt::entity entity);

    any_component &replace(entt::registry &registry, entt::entity entity);

    entt::meta_any any;
};

using meta_type_map = std::unordered_map<ENTT_ID_TYPE, entt::meta_type>;

meta_type_map create_meta_type_map();

/// Supply a function to be called with every reflected component on the entity.
template <typename function_type>
void visit_components(
  entt::registry const &registry,
  entt::entity entity,
  function_type const &function,
  meta_type_map type_map = create_meta_type_map())
{
    registry.visit(entity, [&](auto type_id) {
      auto found_meta_type = type_map.find(type_id);

      if (found_meta_type != type_map.end())
          function(any_component{registry, entity, found_meta_type->second});
    });
}

}
