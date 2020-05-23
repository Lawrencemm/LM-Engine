#pragma once

#include "simulation_plugin.h"
#include <boost/dll.hpp>
#include <string>
#include <vector>

namespace lmeditor
{
class project_plugin
{
  public:
    void load(
      const std::filesystem::path &project_dir,
      entt::meta_ctx const &meta_context);

    psimulation create_simulation(
      size_t simulation_index,
      entt::registry &registry) const;

    std::vector<std::string> const &get_simulation_names()
    {
        return simulation_names;
    };

  private:
    std::filesystem::path project_dir;
    boost::dll::shared_library shared_library;
    std::vector<std::string> simulation_names;
    lmeditor::create_simulation_fn create_simulation_fn;
};
} // namespace lmeditor
