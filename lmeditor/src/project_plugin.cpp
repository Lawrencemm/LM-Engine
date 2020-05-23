#include <lmeditor/project_plugin.h>

namespace lmeditor
{
void project_plugin::load(
  const std::filesystem::path &project_dir,
  entt::meta_ctx const &meta_context)
{
    this->project_dir = project_dir;
    shared_library = boost::dll::shared_library{
      (project_dir / "game").c_str(),
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
  entt::registry &registry) const
{
    return create_simulation_fn(
      simulation_names[simulation_index],
      lmng::simulation_init{registry, project_dir});
}
} // namespace lmeditor
