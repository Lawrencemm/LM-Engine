#pragma once

#include "../mesh_editor.h"

namespace lmeditor
{
template <typename state_type>
void mesh_editor::on_clicked_nothing(state_type &state)
{
    if (!key_state.control())
    {
        clear_selection(state);
    }
}

template <typename state_type, typename handle_type>
void mesh_editor::on_clicked(state_type &state, handle_type handle)
{
    if (key_state.control())
        modify_selection(state, handle);
    else
        select_only(state, handle);
}

template <typename state_type>
void mesh_editor::clear_selection(state_type &state)
{
    for (auto h : state.selection)
        editable_mesh.set_colour(h, unselected_colour);
    state.selection.clear();
}

template <typename state_type, typename handle_type>
void mesh_editor::select_only(state_type &state, handle_type h)
{
    clear_selection(state);

    editable_mesh.set_colour(h, selected_colour);
    state.selection.emplace(h);
}

template <typename state_type, typename handle_type>
void mesh_editor::modify_selection(state_type &state, handle_type h)
{
    auto found_selection = state.selection.find(h);

    if (found_selection != state.selection.end())
    {
        editable_mesh.set_colour(h, unselected_colour);
        state.selection.erase(found_selection);
    }
    else
    {
        editable_mesh.set_colour(h, selected_colour);
        state.selection.emplace(h);
    }
}

template <typename handle_type>
bool mesh_editor::in_selection(handle_type handle)
{
    return state >> lm::variant_visitor{[handle](auto state) {
               return state.in_selection(handle);
           }};
}
} // namespace lmeditor
