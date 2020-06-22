#include <lmng/logging.h>
#include <spdlog/spdlog.h>

namespace lmng
{
void connect_component_logging(entt::registry &registry)
{
    entt::resolve([&](auto meta_type) {
        if (meta_type.prop("is_component"_hs))
        {
            connect_component_logging(meta_type, registry);
        }
    });
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