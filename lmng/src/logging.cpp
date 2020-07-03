#include <lmng/logging.h>
#include <lmng/name.h>
#include <spdlog/spdlog.h>

namespace lmng
{
void log_name_event(
  entt::registry &registry,
  entt::entity entity,
  char const *event_name)
{
    SPDLOG_INFO(
      "{} name '{}' for entity with id {}",
      event_name,
      get_name(registry, entity),
      to_integral(entity));
}

void log_name_construct(entt::registry &registry, entt::entity entity)
{
    log_name_event(registry, entity, "Constructing");
}

void log_name_replace(entt::registry &registry, entt::entity entity)
{
    log_name_event(registry, entity, "Replacing");
}

void log_name_destroy(entt::registry &registry, entt::entity entity)
{
    log_name_event(registry, entity, "Destroying");
}

void connect_component_logging(entt::registry &registry)
{
    entt::resolve([&](auto meta_type) {
        if (meta_type.prop("is_component"_hs))
        {
            connect_component_logging(meta_type, registry);
        }
    });
    registry.on_construct<name>().connect<&log_name_construct>();
    registry.on_replace<name>().connect<&log_name_replace>();
    registry.on_destroy<name>().connect<&log_name_destroy>();
}

void connect_component_logging(
  const entt::meta_type &meta_type,
  entt::registry &registry)
{
    SPDLOG_INFO(
      "Connecting component {} to logging",
      meta_type.prop("name"_hs.value()).value().template cast<char const *>());

    meta_type.func("connect_logging"_hs)
      .invoke({}, entt::meta_any(std::reference_wrapper(registry)));
}
} // namespace lmng