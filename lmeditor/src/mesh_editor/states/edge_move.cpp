#include <lmlib/variant_visitor.h>

#include "../mesh_editor.h"

namespace lmeditor
{

std::tuple<mesh_editor::state_type_optional, bool>
  mesh_editor::handle(EdgeMoveMode &state, lmpl::key_down_message message)
{
    if (message.key == +lmpl::key_code::M)
    {
        return {EdgeSelectMode{std::move(state.selection)}, true};
    }

    return {std::nullopt, false};
}

std::tuple<mesh_editor::state_type_optional, bool> mesh_editor::handle(
  EdgeMoveMode &state,
  const lmtk::mouse_move_event &message)
{
    std::set<editable_mesh::vertex_handle> vhandles;
    for (auto eh : state.selection)
    {
        auto [v1, v2] = editable_mesh.geo_mesh.get_vertices(eh);
        vhandles.emplace(v1);
        vhandles.emplace(v2);
    }
    for (auto vh : vhandles)
    {
        move_vertex(vh, message.delta);
    }
    return {std::nullopt, true};
}
} // namespace lmeditor
