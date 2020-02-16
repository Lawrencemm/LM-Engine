#include <memory>

#include <lmlib/variant_visitor.h>

#include "../mesh_editor.h"

namespace lmeditor
{

std::tuple<mesh_editor::state_type_optional, bool> mesh_editor::handle(
  VertexMoveMode &state,
  const lmtk::mouse_move_event &message)
{
    for (auto vh : state.selection)
    {
        move_vertex(vh, message.delta);
    }

    // Inform parent we must be redrawn.
    return {std::nullopt, true};
}

std::tuple<mesh_editor::state_type_optional, bool>
  mesh_editor::handle(VertexMoveMode &state, lmpl::key_down_message msg)
{
    if (msg.key == +lmpl ::key_code::M)
    {
        return {VertexSelectMode{std::move(state.selection)}, true};
    }
    return {std::nullopt, false};
}
} // namespace lmeditor
