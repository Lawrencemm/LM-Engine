#pragma once
#include <string>
#include <vector>

#include <lmlib/reference.h>
#include <lmtk/lmtk.h>

namespace lmeditor
{
class isimulation
{
  public:
    virtual void handle_input_event(
      lmtk::input_event const &input_event,
      entt::registry &registry) = 0;
    virtual void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state) = 0;
    virtual ~isimulation() = default;
};

using psimulation = lm::reference<isimulation>;

typedef void (*set_meta_context_fn)(entt::meta_ctx const &);
typedef void (*reflect_types_fn)();
typedef std::vector<std::string> (*list_simulations_fn)();
typedef psimulation (
  *create_simulation_fn)(std::string const &, entt::registry &);
} // namespace lmeditor

#define LMEDITOR_SIMULATION_PLUGIN(simulation_class)                           \
    class simulation_class##_plugin : public lmeditor::isimulation             \
    {                                                                          \
        simulation_class underlying_instance;                                  \
                                                                               \
      public:                                                                  \
        explicit simulation_class##_plugin(entt::registry &registry)           \
            : underlying_instance{registry}                                    \
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
