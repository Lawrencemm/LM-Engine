#include <lmng/meta/signal.h>

static void connect_component(
  entt::registry &registry,
  lmng::any_component_listener &listener,
  entt::meta_func const &connect_func)
{
    connect_func.invoke(
      {}, std::reference_wrapper(registry), std::reference_wrapper(listener));
}

static void connect_all_components(
  entt::registry &registry,
  lmng::any_component_listener &listener,
  entt::hashed_string connect_func_name)
{
    for (auto const &component_type : entt::resolve())
    {
        if (!component_type.prop(entt::hashed_string{"is_component"}))
            return;

        connect_component(
          registry, listener, component_type.func(connect_func_name));
    };
}

static void disconnect_component(
  entt::registry &registry,
  lmng::any_component_listener &listener,
  entt::meta_func const &disconnect_func)
{
    disconnect_func.invoke(
      {}, std::reference_wrapper(registry), std::reference_wrapper(listener));
}

static void disconnect_all_components(
  entt::registry &registry,
  lmng::any_component_listener &listener,
  entt::hashed_string disconnect_func_name)
{
    for (auto const &component_type : entt::resolve())
    {
        if (!component_type.prop(entt::hashed_string{"is_component"}))
            return;

        disconnect_component(
          registry, listener, component_type.func(disconnect_func_name));
    };
}

namespace lmng
{
void connect_on_construct_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    connect_all_components(registry, listener, "connect_construct"_hs);
}

void connect_on_update_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    connect_all_components(registry, listener, "connect_replace"_hs);
}

void connect_on_destroy_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    connect_all_components(registry, listener, "connect_destroy");
}

void disconnect_on_construct_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    disconnect_all_components(registry, listener, "disconnect_construct"_hs);
}

void disconnect_on_update_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    disconnect_all_components(registry, listener, "disconnect_replace"_hs);
}

void disconnect_on_destroy_any(
  entt::registry &registry,
  any_component_listener &listener)
{
    disconnect_all_components(registry, listener, "disconnect_destroy");
}
} // namespace lmng