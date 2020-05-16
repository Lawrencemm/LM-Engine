#pragma once
#include <string>
#include <vector>

#include <lmengine/simulation.h>
#include <lmlib/reference.h>
#include <lmtk/input_event.h>

namespace lmeditor
{
class simulation_plugin_interface
{
  public:
    virtual void handle_input_event(
      lmtk::input_event const &input_event,
      entt::registry &registry) = 0;
    virtual void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state) = 0;
    virtual ~simulation_plugin_interface() = default;
};

using psimulation = lm::reference<simulation_plugin_interface>;

typedef void (*set_meta_context_fn)(entt::meta_ctx const &);
typedef void (*reflect_types_fn)();
typedef std::vector<std::string> (*list_simulations_fn)();
typedef psimulation (
  *create_simulation_fn)(std::string const &, lmng::simulation_init const &);
} // namespace lmeditor

#define LMEDITOR_SIMULATION_PLUGIN(simulation_class)                           \
    class simulation_class##_plugin                                            \
        : public lmeditor::simulation_plugin_interface                         \
    {                                                                          \
        simulation_class underlying_instance;                                  \
                                                                               \
      public:                                                                  \
        explicit simulation_class##_plugin(lmng::simulation_init const &init)  \
            : underlying_instance{init}                                        \
        {                                                                      \
        }                                                                      \
        void handle_input_event(                                               \
          lmtk::input_event const &input_event,                                \
          entt::registry &registry) override                                   \
        {                                                                      \
            underlying_instance.handle_input_event(input_event, registry);     \
        }                                                                      \
        void update(                                                           \
          entt::registry &registry,                                            \
          float dt,                                                            \
          lmtk::input_state const &input_state) override                       \
        {                                                                      \
            underlying_instance.update(registry, dt, input_state);             \
        }                                                                      \
    };
