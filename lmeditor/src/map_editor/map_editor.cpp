#include "map_editor.h"
#include "model/model.h"
#include "view/view.h"

#include <fmt/format.h>
#include <range/v3/action/erase.hpp>

#include <lmengine/name.h>
#include <lmengine/reflection.h>

namespace lmeditor
{

pmap_editor map_editor_init::operator()()
{
    return std::make_unique<map_editor>(*this);
}

map_editor::map_editor(map_editor_init const &init)
    : model{{orbital_camera_init{
        .fov = (float)M_PI / 3,
        .aspect = (float)init.size.width / init.size.height,
        .near_clip = 0.1f,
        .far_clip = 1000.f,
        .position = Eigen::Vector3f{0.f, 10.f, -25.f},
        .target = Eigen::Vector3f{0.f, 0.f, 0.f},
      }}},
      view{{init, init.map, model.state_text}}
{
}

bool map_editor::handle(
  entt::registry &map,
  lmtk::input_event const &event,
  lmtk::resource_sink &resource_sink,
  map_editor_event_handler const &event_handler)
{
    return model.handle(map, event, event_handler, [&](auto &model) {
        view.set_state_text(model.state_text, resource_sink);
    });
}

imap_editor &map_editor::set_rect(lm::point2i position, lm::size2i size)
{
    view.set_rect(position, size);
    model.camera.aspect = (float)size.width / (float)size.height;
    return *this;
}

entt::entity map_editor::get_selection() { return model.selected_box; }

map_editor &
  map_editor::move_resources(lmgl::irenderer *, lmtk::resource_sink &sink)
{
    view.move_resources(sink);
    return *this;
}

void map_editor::set_map(
  entt::registry const &map,
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    view.visual_view->clear(renderer, resource_sink);
    model.set_map(map);
    view.visual_view->recreate(map, *renderer);
}

std::vector<command_description> map_editor::get_command_descriptions()
{
    return model.get_command_descriptions();
}

imap_editor &map_editor::select(entt::entity entity)
{
    model.select_box(entity);
    return *this;
}

lm::point2i map_editor::get_position() { return view.position; }

lm::size2i map_editor::get_size() { return view.size; }
} // namespace lmeditor
