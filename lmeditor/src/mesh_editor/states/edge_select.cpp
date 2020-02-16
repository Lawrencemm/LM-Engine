#include "../mesh_editor.h"
#include "select.h"

namespace lmeditor
{
std::tuple<mesh_editor::state_type_optional, bool>
  mesh_editor::handle(EdgeSelectMode &state, lmpl::key_down_message msg)
{
    switch (msg.key)
    {
    case lmpl::key_code::E:
    {
        if (state.selection.empty())
            return {std::nullopt, false};

        std::set<editable_mesh::edge_handle> new_edges;

        for (auto eh : state.selection)
            new_edges.emplace(editable_mesh.geo_mesh.extrude_edge(eh));

        clear_selection(state);

        for (auto eh : new_edges)
            modify_selection(state, eh);

        return {EdgeMoveMode{std::move(state.selection)}, true};
    }

    case lmpl::key_code::V:
        clear_selection(state);
        return {VertexSelectMode{}, true};

    case lmpl::key_code::M:
        if (!state.selection.empty())
            return {EdgeMoveMode{std::move(state.selection)}, true};
        break;

    default:
        break;
    }
    return {std::nullopt, false};
}

std::tuple<mesh_editor::state_type_optional, bool> mesh_editor::handle(
  EdgeSelectMode &state,
  const lmpl::mouse_button_down_message msg)
{
    switch (msg.button)
    {
    case lmpl::mouse_button::L:
    {
        auto hit_edge = edge_hit_test(msg.pos, 5);

        if (hit_edge.is_valid())
            on_clicked(state, hit_edge);
        else
            on_clicked_nothing(state);

        break;
    }

    default:
        break;
    }

    return {std::nullopt, false};
}
} // namespace lmeditor
