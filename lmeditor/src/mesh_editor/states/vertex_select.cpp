#include <memory>

#include <lmlib/variant_visitor.h>

#include "../mesh_editor.h"
#include "select.h"

namespace lmeditor
{
std::tuple<mesh_editor::state_type_optional, bool>
  mesh_editor::handle(VertexSelectMode &state, lmpl::key_down_message msg)
{
    switch (msg.key)
    {

    case lmpl::key_code::E:
        clear_selection(state);
        return {EdgeSelectMode{}, true};

    case lmpl::key_code::M:
        if (!state.selection.empty())
            return {VertexMoveMode{std::move(state.selection)}, true};
        break;

    default:
        break;
    }
    return {std::nullopt, false};
}

std::tuple<mesh_editor::state_type_optional, bool> mesh_editor::handle(
  VertexSelectMode &state,
  lmpl::mouse_button_down_message msg)
{

    switch (msg.button)
    {
    case lmpl::mouse_button::L:
    {
        auto hit_vertex = vertex_hit_test(get_surface_point_real(msg.pos), 5);

        if (hit_vertex.is_valid())
            on_clicked(state, hit_vertex);
        else
            on_clicked_nothing(state);

        return {std::nullopt, true};
    }

    default:
        break;
    }

    return {std::nullopt, false};
}
} // namespace lmeditor
