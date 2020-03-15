#pragma once

#include "tool_panel.h"

namespace lmeditor
{
template <typename widget_type> struct widget_panel_wrapper : public itool_panel
{
    widget_type widget;

    explicit widget_panel_wrapper(widget_type &&widget)
        : widget{std::move(widget)}
    {
    }

    itool_panel &
      add_to_frame(lmgl::iframe *frame, editor_app const &app) override
    {
        widget.add_to_frame(frame);
        return *this;
    }
    std::vector<command_description> get_command_descriptions() override
    {
        return std::vector<command_description>();
    }
    lm::size2i get_size() override { return widget.get_size(); }
    lm::point2i get_position() override { return widget.get_position(); }
    iwidget &set_rect(lm::point2i position, lm::size2i size) override
    {
        widget.set_rect(position, size);
        return *this;
    }
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override
    {
        widget.move_resources(renderer, resource_sink);
        return *this;
    }
};
} // namespace lmeditor
