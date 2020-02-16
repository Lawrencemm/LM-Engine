#pragma once

#include <optional>

#include <lmlib/geometry.h>
#include <lmpl/keyboard.h>
#include <lmpl/mouse.h>

namespace lmtk
{
struct input_state
{
    lmpl::key_state key_state;
    lmpl::mouse_state mouse_state;
    std::optional<lm::point2i> mouse_pos;

    void reset()
    {
        key_state = lmpl::key_state{};
        mouse_state = lmpl::mouse_state{};
        mouse_pos = std::nullopt;
    }
};
} // namespace lmtk
