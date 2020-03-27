#include <lmeditor/map_selector.h>
#include <lmtk/vlayout.h>
#include <range/v3/action/slice.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

std::vector<std::filesystem::path>
  get_map_files(std::filesystem::path const &dir)
{
    return std::filesystem::recursive_directory_iterator{dir} |
           ranges::views::filter(
             [](auto &entry) { return entry.path().extension() == ".lmap"; }) |
           ranges::views::transform([&](auto &entry) {
               auto with_ext = std::filesystem::relative(entry, dir).string();
               return with_ext.substr(
                 0, with_ext.size() - std::string{".lmap"}.size());
           }) |
           ranges::to<std::vector<std::filesystem::path>>();
}

namespace lmeditor
{
std::vector<lmtk::text_layout> create_entries(
  lmgl::irenderer *renderer,
  lmgl::material font_material,
  lmtk::ifont const *font,
  std::vector<std::filesystem::path> const &paths)
{
    std::vector<lmtk::text_layout> entries;
    for (auto &path : paths)
    {
        entries.emplace_back(lmtk::text_layout_init{
          .renderer = *renderer,
          .material = font_material,
          .font = font,
          .colour = {1.f, 1.f, 1.f},
          .text = path.string(),
        });
    }
    lmtk::layout_vertical(
      lmtk::vertical_layout{.start = 0, .spacing = 16}, entries);
    return std::move(entries);
}

map_selector::map_selector(map_selector_init const &init)
    : file_paths{get_map_files(init.directory)},
      selected_map{0},
      entries{create_entries(
        init.renderer,
        init.font_material,
        init.font,
        file_paths)},
      selection_background{
        *init.renderer,
        init.rect_material,
        file_paths.size() > 0 ? entries[selected_map].position
                              : lm::point2i{0, 0},
        file_paths.size() > 0 ? entries[selected_map].get_size()
                              : lm::size2i{0, 0},
        {0.f, 0.f, 0.f, 1.f},
      }
{
}

bool map_selector::handle(
  lmtk::input_event const &input_event,
  map_selector_event_handler const &event_handler)
{
    return input_event >> lm::variant_visitor{
                            [&](lmtk::key_down_event const &key_down_event) {
                                switch (key_down_event.key)
                                {
                                case lmpl::key_code::Enter:
                                    event_handler(map_selector_chose_map{
                                      file_paths[selected_map]});
                                    return true;

                                case lmpl::key_code::I:
                                    if (selected_map == 0)
                                        return false;

                                    selected_map -= 1;
                                    return true;

                                case lmpl::key_code::K:
                                    if (selected_map + 1 == entries.size())
                                        return false;

                                    selected_map += 1;
                                    return true;

                                default:
                                    return false;
                                }
                            },
                            [](auto &) { return false; },
                          };
}

map_selector &map_selector::add_to_frame(lmgl::iframe *frame)
{
    if (file_paths.size() > 0)
    {
        selection_background.set_rect(
          entries[selected_map].position, entries[selected_map].get_size());
        selection_background.add_to_frame(frame);
    }
    for (auto &layout : entries)
    {
        layout.render(frame);
    }
    return *this;
}

lm::size2i map_selector::get_size() { return lm::size2i(); }

lm::point2i map_selector::get_position() { return lm::point2i(); }

map_selector &map_selector::set_rect(lm::point2i position, lm::size2i size)
{
    return *this;
}

map_selector &map_selector::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    for (auto &layout : entries)
        layout.move_resources(renderer, resource_sink);

    selection_background.move_resources(renderer, resource_sink);
    return *this;
}
} // namespace lmeditor
