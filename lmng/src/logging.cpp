#include <lmng/logging.h>
#include <lmng/meta/any_component.h>
#include <lmng/name.h>
#include <spdlog/spdlog.h>

namespace lmng
{
void connect_component_logging(entt::registry &registry)
{
    for (auto const &meta_type : entt::resolve())
    {
        if (meta_type.prop("is_component"_hs))
        {
            connect_component_logging(meta_type, registry);
        }
    };
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

std::string output_data_for_log(
  entt::meta_any const &component,
  entt::registry const &context)
{
    std::string data_str{};

    bool has_data{false};
    for (auto data : component.type().data())
    {
        has_data = true;
        data_str += fmt::format(
          "{}: {}, ",
          lmng::get_data_name(data),
          get_data(component, data, context));
    };

    if (has_data)
    {
        data_str.erase(data_str.end() - 2, data_str.end());
        return fmt::format(" {} ", data_str);
    }

    return " ";
}
} // namespace lmng