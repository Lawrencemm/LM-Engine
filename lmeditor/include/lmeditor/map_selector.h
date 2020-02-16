#pragma once

#include <filesystem>
#include <functional>
#include <lmtk/lmtk.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
struct map_selector_chose_map;
struct map_selector_event_handler;
struct map_selector_init;

class map_selector : public lmtk::iwidget
{
  public:
    map_selector(map_selector_init const &init);
    bool handle(
      lmtk::input_event const &input_event,
      map_selector_event_handler const &event_handler);
    iwidget &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;

  private:
    std::vector<std::filesystem::path> file_paths;
    int selected_map;
    std::vector<lmtk::text_layout> entries;
    lmtk::rect selection_background;
};

struct map_selector_event_handler
{
    using chose_map_handler =
      std::function<void(map_selector_chose_map const &)>;

    chose_map_handler on_chose_map;

    void operator()(map_selector_chose_map const &event) const
    {
        on_chose_map(event);
    }
};

struct map_selector_chose_map
{
    std::filesystem::path const &path_to_file;
};

struct map_selector_init
{
    std::filesystem::path const &directory;
    lmgl::irenderer *renderer;
    lmgl::material font_material;
    lmtk::ifont const *font;
    lmgl::material rect_material;
};
} // namespace lmeditor
