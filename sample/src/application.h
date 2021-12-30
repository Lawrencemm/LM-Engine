#pragma once

#include "simulations/character_movement.h"
#include <entt/entity/registry.hpp>
#include <lmhuv.h>
#include <lmlib/realtime_clock.h>
#include <lmtk/app.h>
#include <lmtk/event.h>

class sample_app : public lmtk::app
{
  public:
    sample_app();

  protected:
  private:
    bool on_event(const lmtk::event &event) override;

  protected:
    lmng::asset_cache asset_cache;

    entt::registry registry;
    character_movement simulation;
    lmhuv::pvisual_view visual_view;

    lm::realtime_clock clock;
};
