#pragma once

#include <filesystem>
#include <variant>

#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <lmgl/lmgl.h>
#include <lmlib/geometry.h>
#include <lmtk/lmtk.h>
#include <lmtk/text_layout.h>

#include "command_help.h"
#include "tool_panel.h"

namespace lmeditor
{
struct map_editor_init
{
    lmgl::irenderer *renderer;
    lm::point2i position;
    lm::size2i size;
    std::array<float, 3> selection_outline_colour;
    lmgl::material text_material;
    lmtk::ifont const *font;
};

struct map_editor_event_handler;

struct imap_editor : public itool_panel
{
    virtual bool handle(
      lmtk::input_event const &input_event,
      lmtk::resource_sink &resource_sink,
      map_editor_event_handler const &event_handler) = 0;
    virtual bool update_selection(
      entt::meta_data const &data,
      std::string const &string_repr) = 0;
    virtual void add_component_to_selected(
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) = 0;
    virtual void remove_component_from_selected(
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) = 0;
    virtual entt::registry &get_map() = 0;
    virtual void load_map(
      std::filesystem::path const &file_path,
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) = 0;
    virtual void save_map(std::filesystem::path const &file_path) = 0;
    virtual entt::entity get_selection() = 0;
    virtual imap_editor &select(entt::entity) = 0;
    virtual ~imap_editor() = default;
};

using pmap_editor = lm::reference<imap_editor>;

pmap_editor create_map_editor(const map_editor_init &init);
} // namespace lmeditor
