#pragma once

#include "simulations/character_movement.h"
#include <entt/entity/registry.hpp>
#include <lmhuv.h>
#include <lmlib/realtime_clock.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/input_event.h>

class sample_app
{
  public:
    sample_app();
    void main() { flow_graph.enter(); }

  protected:
    bool on_input_event(lmtk::input_event const &variant);
    bool on_new_frame(lmgl::iframe *pIframe);
    void on_quit();

    lmtk::app_resources resources;
    lmtk::app_flow_graph flow_graph;
    lmng::asset_cache asset_cache;

    entt::registry registry;
    character_movement simulation;
    lmhuv::pvisual_view visual_view;

    lm::realtime_clock clock;
};
