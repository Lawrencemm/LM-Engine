#include <catch2/catch.hpp>

#include <lmtk/test.h>

#include "../test_mesh_editor.h"
#include "select.h"

using namespace lmeditor;

TEST_CASE_METHOD(MeshEditorTestCase, "Vertex Select Mode")
{
    lmtk::input_state input_state;

    REQUIRE(
      std::set{fill_colour, unselected_colour, selected_colour}.size() == 3);

    SECTION("Vertices are initially unselected")
    {
        auto vh = lmeditor::trimesh::VertexHandle{GENERATE(0, 1, 2, 3)};

        REQUIRE(mesh.color(vh) == unselected_colour);
    }

    SECTION("No vertex is selected after clicking empty space")
    {
        auto vh = lmeditor::trimesh::VertexHandle{GENERATE(0, 1, 2, 3)};

        click(mesh_editor, {0, 0}, input_state);

        REQUIRE(mesh.color(vh) == unselected_colour);
    }

    SECTION("Vertex is selected after being clicked")
    {
        auto vh = editable_mesh::vertex_handle{0};

        auto vpoint = mesh_editor.point(vh);

        click(mesh_editor, vpoint, input_state);

        REQUIRE(mesh_editor.in_selection(vh));
        REQUIRE(mesh.color(vh) == selected_colour);

        SECTION("Vertex is unselected after clicking empty space")
        {
            click(mesh_editor, vpoint + lm::point2i{15, 0}, input_state);

            REQUIRE(!mesh_editor.in_selection(vh));
            REQUIRE(mesh.color(vh) == unselected_colour);
        }

        SECTION("Vertex is unselected after clicking another")
        {
            auto v2h = editable_mesh::vertex_handle{1};

            auto v2point = mesh_editor.point(v2h);

            click(mesh_editor, v2point, input_state);

            REQUIRE(mesh_editor.in_selection(v2h));
            REQUIRE(mesh.color(v2h) == selected_colour);
            REQUIRE(!mesh_editor.in_selection(vh));
            REQUIRE(mesh.color(vh) == unselected_colour);
        }

        SECTION("Vertex is selected after clicking another with control held")
        {
            auto v2h = editable_mesh::vertex_handle{1};

            auto v2point = mesh_editor.point(v2h);

            press(mesh_editor, lmpl::key_code::LeftControl, input_state);
            click(mesh_editor, v2point, input_state);

            REQUIRE(mesh_editor.in_selection(vh));
            REQUIRE(mesh.color(vh) == selected_colour);

            SECTION("Vertex is selected after clicking with control held")
            {
                REQUIRE(mesh_editor.in_selection(v2h));
                REQUIRE(mesh.color(v2h) == selected_colour);
            }

            SECTION(
              "Vertices remain selected clicking empty space with control held")
            {
                click(mesh_editor, vpoint + lm::point2i{15, 0}, input_state);
                REQUIRE(mesh_editor.in_selection(vh));
                REQUIRE(mesh.color(vh) == selected_colour);
                REQUIRE(mesh_editor.in_selection(v2h));
                REQUIRE(mesh.color(v2h) == selected_colour);
            }

            SECTION("Vertex is deselected after clicking with control held")
            {
                click(mesh_editor, vpoint, input_state);

                REQUIRE(!mesh_editor.in_selection(vh));
                REQUIRE(mesh.color(vh) == unselected_colour);

                SECTION("Other vertex is still selected after clicking "
                        "selected vertex with control held")
                {
                    REQUIRE(mesh_editor.in_selection(v2h));
                    REQUIRE(mesh.color(v2h) == selected_colour);
                }
            }

            SECTION("All vertices are deselected after clicking empty space")
            {
                lmtk::lift(
                  mesh_editor, lmpl::key_code::LeftControl, input_state);
                click(mesh_editor, vpoint + lm::point2i{15, 0}, input_state);

                REQUIRE(!mesh_editor.in_selection(vh));
                REQUIRE(!mesh_editor.in_selection(v2h));

                REQUIRE(mesh.color(vh) == unselected_colour);
                REQUIRE(mesh.color(v2h) == unselected_colour);
            }
        }
    }
}
