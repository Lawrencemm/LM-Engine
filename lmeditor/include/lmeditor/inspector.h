#pragma once

#include <variant>

#include <entt/meta/meta.hpp>

#include <lmlib/reference.h>
#include <lmtk/lmtk.h>
#include <lmtk/text_layout.h>

#include "tool_panel.h"

namespace lmeditor
{
struct inspector_updated_data
{
    entt::meta_data data;
    std::string string_repr;
};
struct inspector_added_component
{
    entt::meta_type type;
};
struct inspector_changed_name
{
    std::string new_name;
};
struct inspector_removed_component
{
    entt::entity entity;
    entt::meta_type component_type;
};

struct inspector_event_handler
{
    using updated_data_handler =
      std::function<void(inspector_updated_data const &)>;
    using added_component_handler =
      std::function<void(inspector_added_component const &)>;
    using changed_name_handler =
      std::function<void(inspector_changed_name const &)>;
    using removed_component_handler =
      std::function<void(inspector_removed_component const &)>;

    updated_data_handler on_updated_data{[](auto &) {}};
    added_component_handler on_added_component{[](auto &) {}};
    changed_name_handler on_changed_name{[](auto &) {}};
    removed_component_handler on_removed_component{[](auto &) {}};

    void operator()(inspector_updated_data const &event) const
    {
        on_updated_data(event);
    }
    void operator()(inspector_added_component const &event) const
    {
        on_added_component(event);
    }
    void operator()(inspector_changed_name const &event) const
    {
        on_changed_name(event);
    }
    void operator()(inspector_removed_component const &event) const
    {
        on_removed_component(event);
    }
};

struct iinspector : public itool_panel
{
    virtual bool handle(
      lmtk::input_event const &input_event,
      lmtk::resource_sink &resource_sink,
      inspector_event_handler const &event_handler) = 0;
    virtual void display(
      entt::registry &registry,
      entt::entity entity,
      lmtk::resource_sink &resource_sink) = 0;
    virtual void
      update(entt::registry &registry, lmtk::resource_sink &resource_sink) = 0;
    virtual void clear(lmtk::resource_sink &resource_sink) = 0;
    virtual ~iinspector() = default;
};

using pinspector = lm::reference<iinspector>;

pinspector create_inspector(
  lmgl::irenderer &renderer,
  lmgl::material text_material,
  lmtk::ifont const *font,
  lm::size2i const &size);

} // namespace lmeditor
