#pragma once

#include <range/v3/view/enumerate.hpp>

#include <lmlib/geometry.h>

namespace lmtk
{
struct vertical_layout
{
    int start;
    int spacing;
};

template <typename range_type>
void layout_vertical(vertical_layout const &layout, range_type &widgets)
{
    int current{layout.start};
    for (auto [i, widget] : ranges::views::enumerate(widgets))
    {
        if (i)
            current +=
              (widgets.begin() + i - 1)->get_size().height + layout.spacing;

        lm::point2i widget_pos = widget.position;
        widget_pos.y = current;
        widget.set_position(widget_pos);
    }
}

template <typename range_type>
void layout_vertical(vertical_layout const &layout, range_type const &widgets)
{
    int current{layout.start};
    for (auto [i, widget] : ranges::views::enumerate(widgets))
    {
        if (i)
            current +=
              (widgets.begin() + i - 1)->get_size().height + layout.spacing;

        lm::point2i widget_pos = widget.position;
        widget_pos.y = current;
        widget.set_position(widget_pos);
    }
}

template <typename range_type>
void layout_vertical_pointers(
  vertical_layout const &layout,
  range_type &widgets)
{
    int current{layout.start};
    for (auto [i, widget] : ranges::views::enumerate(widgets))
    {
        if (i)
            current +=
              (*(widgets.begin() + i - 1))->get_size().height + layout.spacing;

        lm::point2i widget_pos = widget->position;
        widget_pos.y = current;
        widget->set_position(widget_pos);
    }
}
} // namespace lmtk
