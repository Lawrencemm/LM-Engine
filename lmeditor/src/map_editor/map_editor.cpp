#include "model/model.h"
#include "view/view.h"

#include <fmt/format.h>
#include <range/v3/action/erase.hpp>

#include <lmengine/name.h>
#include <lmengine/reflection.h>

namespace lmeditor
{
class map_editor : public imap_editor
{
    map_editor_model model;
    map_editor_view view;

  public:
    map_editor(map_editor_init const &init)
        : model{{orbital_camera_init{
            .fov = (float)M_PI / 3,
            .aspect = (float)init.size.width / init.size.height,
            .near_clip = 0.1f,
            .far_clip = 1000.f,
            .position = Eigen::Vector3f{0.f, 10.f, -25.f},
            .target = Eigen::Vector3f{0.f, 0.f, 0.f},
          }}},
          view{{init, model.map, model.state_text}}
    {
    }

    bool handle(
      lmtk::input_event const &event,
      lmtk::resource_sink &resource_sink,
      map_editor_event_handler const &event_handler) override
    {
        lmhuv::visual_view_connections view_connections{
          *view.visual_view, model.map, *view.renderer, resource_sink};

        return model.handle(event, event_handler, [&](auto &model) {
            view.set_state_text(model.state_text, resource_sink);
        });
    }

    void remove_component_from_selected(
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) override
    {
        lmhuv::visual_view_connections view_connections{
          *view.visual_view, model.map, *view.renderer, resource_sink};

        lmng::remove_from_entity(type, model.map, model.selected_box);
    }

    bool update_selection(
      entt::meta_data const &data,
      std::string const &string_repr) override
    {
        return model.update_selection(data, string_repr);
    };

    void add_component_to_selected(
      entt::meta_type const &type,
      lmtk::resource_sink &resource_sink) override
    {
        lmhuv::visual_view_connections view_connections{
          *view.visual_view, model.map, *view.renderer, resource_sink};
        model.add_component_to_selected(type);
    };

    entt::registry &get_map() override { return model.map; };

    map_editor &add_to_frame(lmgl::iframe *frame) override
    {
        view.render(frame, model);
        return *this;
    }

    imap_editor &set_rect(lm::point2i position, lm::size2i size) override
    {
        view.set_rect(position, size);
        model.camera.aspect = (float)size.width / (float)size.height;
        return *this;
    }

    entt::entity get_selection() override { return model.selected_box; };

    map_editor &
      move_resources(lmgl::irenderer *, lmtk::resource_sink &sink) override
    {
        view.move_resources(sink);
        return *this;
    };

    void load_map(
      std::filesystem::path const &file_path,
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override
    {
        view.visual_view->clear(renderer, resource_sink);
        model.load_map(file_path);
        view.visual_view->recreate(model.map, *renderer);
    }
    void save_map(std::filesystem::path const &file_path) override
    {
        model.save_map(file_path);
    }
    std::vector<command_description> get_command_descriptions() override
    {
        return model.get_command_descriptions();
    }
    lm::size2i get_size() override { return view.size; }
    lm::point2i get_position() override { return view.position; }
    imap_editor &select(entt::entity entity) override
    {
        model.select_box(entity);
        return *this;
    }
}; // namespace lmeditor

pmap_editor create_map_editor(map_editor_init const &init)
{
    return std::make_unique<map_editor>(init);
}
} // namespace lmeditor
