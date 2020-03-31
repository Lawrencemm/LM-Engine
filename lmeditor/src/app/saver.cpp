#include "saver.h"
#include "resources.h"
#include <entt/entt.hpp>

namespace lmeditor
{
saver::saver(
  editor_app_resources &resources,
  std::string const &header,
  std::string const &initial)
    : header{resources.create_text_layout(header)},
      field{resources.create_char_field(initial)}
{
    field.set_rect({0, this->header.get_size().height + 15}, {0, 0});
}

saver &saver::add_to_frame(lmgl::iframe *frame)
{
    header.render(frame);
    field.add_to_frame(frame);
    return *this;
}

saver &saver::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    header.move_resources(renderer, resource_sink);
    field.move_resources(renderer, resource_sink);
    return *this;
}

saver &saver::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
    return *this;
}
} // namespace lmeditor
