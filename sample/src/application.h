#pragma once

#include "simulations/rigid_bodies_battle.h"
#include <entt/entity/registry.hpp>
#include <lmhuv.h>
#include <lmlib/realtime_clock.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/lmtk.h>

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

    entt::registry registry;
    rigid_bodies_battle simulation;
    lmhuv::pvisual_view visual_view;

    lm::realtime_clock clock;
};
