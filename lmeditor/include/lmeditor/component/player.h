#pragma once

#include "component.h"
#include <lmeditor/project_plugin.h>
#include <lmeditor/simulation_plugin.h>
#include <lmhuv.h>
#include <lmlib/realtime_clock.h>

namespace lmeditor
{
struct player_init;
class player : public component_interface
{
  public:
    explicit player(player_init const &init);

    std::vector<command_description> get_command_descriptions() override;
    lmtk::component_state handle(const lmtk::event &event) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    lmtk::component_interface &
      set_rect(lm::point2i position, lm::size2i size) override;

    lmtk::component_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

  private:
    entt::registry registry;
    lm::point2i position;
    lm::size2i size;
    psimulation simulation;
    lmhuv::pvisual_view visual_view;

    lm::realtime_clock clock;
};

struct player_init
{
    lmgl::irenderer *renderer;
    entt::registry const &map;
    lmeditor::project_plugin const &project_plugin;
    size_t simulation_index;
    lmng::asset_cache &asset_cache;
    lm::point2i position;
    lm::size2i size;

    lm::reference<player> unique() { return std::make_unique<player>(*this); }
};
} // namespace lmeditor