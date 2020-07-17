#pragma once

#include <entt/entt.hpp>

namespace lmng
{
class any_component_listener
{
  public:
    virtual void on_construct_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) = 0;
    virtual void on_replace_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) = 0;
    virtual void on_destroy_any(
      entt::registry &registry,
      entt::entity entity,
      entt::meta_type const &meta_type) = 0;
};

void connect_on_construct_any(
  entt::registry &registry,
  any_component_listener &listener);

void connect_on_replace_any(
  entt::registry &registry,
  any_component_listener &listener);

void connect_on_destroy_any(
  entt::registry &registry,
  any_component_listener &listener);

void disconnect_on_construct_any(
  entt::registry &registry,
  any_component_listener &listener);

void disconnect_on_replace_any(
  entt::registry &registry,
  any_component_listener &listener);

void disconnect_on_destroy_any(
  entt::registry &registry,
  any_component_listener &listener);
} // namespace lmng
