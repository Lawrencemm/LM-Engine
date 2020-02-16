#include "map_saver.h"

namespace lmeditor
{
map_saver::map_saver(
  editor_app_resources &resources,
  std::string const &initial)
    : header{resources.create_text_layout("Save map")},
      field{resources.create_char_field(initial)}
{
    field.set_rect({0, header.get_size().height + 15}, {0, 0});
}

bool map_saver::handle(
  editor_app &app,
  lmtk::iwidget *widget,
  lmtk::input_event const &input_event)
{
    auto saver = dynamic_cast<map_saver *>(widget);
    auto maybe_key_down_msg = std::get_if<lmtk::key_down_event>(&input_event);
    if (maybe_key_down_msg && maybe_key_down_msg->key == lmpl::key_code::Enter)
    {
        auto relative = saver->field.get_value();
        auto absolute = app.resources.project_dir / (relative + ".lmap");
        app.resources.map_editor->save_map(absolute);
        app.map_file_project_relative_path = relative;
        app.state.emplace<gui_state>(app);
        return true;
    }

    return saver->field.handle(
      input_event,
      app.resources.renderer.get(),
      app.resources.font.get(),
      app.resources.resource_sink);
}

lmtk::iwidget &map_saver::add_to_frame(lmgl::iframe *frame)
{
    header.render(frame);
    field.add_to_frame(frame);
    return *this;
}

lmtk::iwidget &map_saver::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    header.move_resources(renderer, resource_sink);
    field.move_resources(renderer, resource_sink);
    return *this;
}

lmtk::iwidget &map_saver::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
    return *this;
}
} // namespace lmeditor
