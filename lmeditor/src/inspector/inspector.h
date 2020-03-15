#pragma once

#include <entt/entity/registry.hpp>
#include <entt/meta/meta.hpp>
#include <lmeditor/inspector.h>
#include <lmengine/reflection.h>
#include <lmtk/char_field.h>
#include <lmtk/lmtk.h>
#include <lmtk/shapes.h>
#include <lmtk/text_editor.h>

namespace lmeditor
{
class inspector : public iinspector
{
  public:
    inspector(
      lmgl::irenderer &renderer,
      lmgl::material text_material,
      lmtk::ifont const *font,
      lm::size2i const &size);

    bool handle(
      lmtk::input_event const &event,
      lmtk::resource_sink &resource_sink,
      inspector_event_handler const &event_handler) override;

    void update(entt::registry &registry, lmtk::resource_sink &resource_sink)
      override;

    void display(
      entt::registry const &registry,
      entt::entity entity,
      lmtk::resource_sink &resource_sink) override;

    void clear(lmtk::resource_sink &resource_sink) override;

    inspector &
      add_to_frame(lmgl::iframe *frame, editor_app const &app) override;

    lm::size2i get_size() override;
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;

    lmtk::iwidget &
      move_resources(lmgl::irenderer *, lmtk::resource_sink &sink) override;

    std::vector<command_description> get_command_descriptions() override;

    static std::string
      format_component_data(std::string const &name, std::string const &repr);

    lmtk::char_field &selected_field()
    {
        return data_fields[selected_label().data_index];
    }

    bool move_selection(int movement, lmtk::resource_sink &resource_sink);
    void create_text(entt::registry const &registry);
    void clear_text(lmtk::resource_sink &resource_sink);

    struct state_handle_args
    {
        lmeditor::inspector &inspector;
        lmtk::input_event const &input_event;
        lmtk::resource_sink &resource_sink;
        inspector_event_handler const &event_handler;
    };

    struct empty_state
    {
        bool handle(state_handle_args const &args);
    };
    struct select_state
    {
        bool handle(state_handle_args const &args);
    };
    struct edit_state
    {
        bool handle(state_handle_args const &args);
    };
    // Choosing a component type to add.
    struct add_state
    {
        bool handle(state_handle_args const &args);
        bool move_selection(
          inspector &inspector,
          int movement,
          lmtk::resource_sink &resource_sink);
        void render(lmgl::iframe *frame);
        void move_resources(
          lmgl::irenderer *renderer,
          lmtk::resource_sink &resource_sink);

        lmtk::rect background;
        lmtk::text_layout title_line;
        struct entry
        {
            entt::meta_type type;
            lmtk::text_layout line;
        };
        std::vector<entry> entries;
        int selected_entry_index{0};

        entry &selection() { return entries[selected_entry_index]; }
    };

    add_state create_add_state();

    using state_variant_type =
      std::variant<empty_state, select_state, edit_state, add_state>;
    state_variant_type state;

    lmgl::irenderer &renderer;
    lm::point2i position;
    lm::point2i get_position() override;
    lm::size2i size;
    lmgl::material text_material, background_material;
    lmtk::ifont const *font;
    lmtk::rect selection_background;

    struct edit_handler
    {
        std::function<void(std::string, inspector_event_handler const &)>
          on_save;
    };

    void save_selected_field(inspector_event_handler const &event_handler)
    {
        field_edit_handlers[selected_label().data_index].on_save(
          selected_field().get_value(), event_handler);
    }

    entt::entity entity;
    struct label
    {
        lmtk::text_layout text;
        bool is_data;
        size_t data_index;
        size_t component_index;
    };
    std::vector<label> labels;
    std::vector<lmtk::char_field> data_fields;
    std::vector<edit_handler> field_edit_handlers;
    std::vector<entt::meta_type> component_meta_types;
    int selected_label_index{0};

    label &selected_label() { return labels[selected_label_index]; }
    lmtk::text_layout &selected_label_text() { return selected_label().text; }

    void render_base(lmgl::iframe *frame);
    void update_selection_background();
};
} // namespace lmeditor
