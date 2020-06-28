#include <lmeditor/project_plugin.h>

namespace lmeditor
{
void project_plugin::load(
  std::string const &plugin_name,
  entt::meta_ctx const &meta_context)
{
    shared_library = boost::dll::shared_library{
      plugin_name,
      boost::dll::load_mode::append_decorations,
    };
    simulation_names =
      shared_library.get<list_simulations_fn>("list_simulations")();
    create_simulation_fn =
      shared_library.get<lmeditor::create_simulation_fn>("create_simulation");

    shared_library.get<set_meta_context_fn>("set_meta_context")(meta_context);
    shared_library.get<reflect_types_fn>("reflect_types")();
}

psimulation project_plugin::create_simulation(
  size_t simulation_index,
  entt::registry &registry,
  lmng::asset_cache &asset_cache) const
{
    return create_simulation_fn(
      simulation_names[simulation_index],
      lmng::simulation_init{registry, asset_cache});
}
} // namespace lmeditor
