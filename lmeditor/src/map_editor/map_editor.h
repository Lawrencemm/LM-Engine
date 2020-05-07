#pragma once

#include "model/model.h"
#include "view/view.h"
#include <lmeditor/map_editor.h>

namespace lmeditor
{
class map_editor : public imap_editor
{
    map_editor_model model;
    map_editor_view view;

  public:
    map_editor(map_editor_init const &init);

    bool handle(
      entt::registry &map,
      lmtk::input_event const &event,
      lmtk::resource_sink &resource_sink,
      map_editor_event_handler const &event_handler) override;

    map_editor &add_to_frame(lmgl::iframe *frame, editor_app &app) override;

    imap_editor &set_rect(lm::point2i position, lm::size2i size) override;

    entt::entity get_selection() override;

    void remove_component_from_selected(
      entt::registry &map,
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) override;

    bool update_selection(
      entt::registry &map,
      entt::meta_data const &data,
      std::string const &string_repr) override;

    void add_component_to_selected(
      entt::registry &map,
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) override;

    map_editor &
      move_resources(lmgl::irenderer *, lmtk::resource_sink &sink) override;

    void set_map(
      entt::registry const &map,
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;

    std::vector<command_description> get_command_descriptions() override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    imap_editor &select(entt::entity entity) override;
};
} // namespace lmeditor
