#include <catch2/catch.hpp>

#include <lmtk/test.h>

#include "../test_mesh_editor.h"
#include "select.h"

using namespace lmeditor;

TEST_CASE_METHOD(MeshEditorTestCase, "Edge Select Mode")
{
    lmtk::input_state input_state;

    hit(mesh_editor, lmpl::key_code::E, input_state);

    SECTION("Vertices are initially unselected")
    {
        auto vh = editable_mesh::edge_handle{GENERATE(0, 1, 2, 3)};

        REQUIRE(mesh.color(vh) == unselected_colour);
    }

    SECTION("No edge is selected after clicking empty space")
    {
        auto vh = editable_mesh::edge_handle{GENERATE(0, 1, 2, 3)};

        click(mesh_editor, {0, 0}, input_state);

        REQUIRE(mesh.color(vh) == unselected_colour);
    }

    SECTION("Edge is selected after being clicked")
    {
        auto eh = editable_mesh::edge_handle{0};

        auto epoint = mesh_editor.point(eh);

        click(mesh_editor, epoint, input_state);

        REQUIRE(mesh_editor.in_selection(eh));
        REQUIRE(mesh.color(eh) == selected_colour);

        SECTION("Edge colour changes to unselected after clicking empty space")
        {
            click(mesh_editor, epoint + lm::point2i{15, 0}, input_state);

            REQUIRE(!mesh_editor.in_selection(eh));
            REQUIRE(mesh.color(eh) == unselected_colour);
        }

        SECTION("Edge has unselected colour after clicking another")
        {
            auto e2h = editable_mesh::edge_handle{1};

            auto e2point = mesh_editor.point(e2h);

            click(mesh_editor, e2point, input_state);

            REQUIRE(mesh_editor.in_selection(e2h));
            REQUIRE(mesh.color(e2h) == selected_colour);
            REQUIRE(!mesh_editor.in_selection(eh));
            REQUIRE(mesh.color(eh) == unselected_colour);
        }

        SECTION(
          "Edge has selected colour after clicking another with control held")
        {
            auto v2h = editable_mesh::edge_handle{1};

            auto v2point = mesh_editor.point(v2h);

            press(mesh_editor, lmpl::key_code::LeftControl, input_state);
            click(mesh_editor, v2point, input_state);

            REQUIRE(mesh_editor.in_selection(eh));
            REQUIRE(mesh.color(eh) == selected_colour);

            SECTION("Edge is selected after clicking with control held")
            {
                REQUIRE(mesh_editor.in_selection(v2h));
                REQUIRE(mesh.color(v2h) == selected_colour);
            }

            SECTION(
              "Vertices remain selected clicking empty space with control held")
            {
                click(mesh_editor, epoint + lm::point2i{15, 0}, input_state);
                REQUIRE(mesh_editor.in_selection(eh));
                REQUIRE(mesh.color(eh) == selected_colour);
                REQUIRE(mesh_editor.in_selection(v2h));
                REQUIRE(mesh.color(v2h) == selected_colour);
            }

            SECTION("Edge is deselected after clicking with control held")
            {
                click(mesh_editor, epoint, input_state);

                REQUIRE(!mesh_editor.in_selection(eh));
                REQUIRE(mesh.color(eh) == unselected_colour);

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
                click(mesh_editor, epoint + lm::point2i{15, 0}, input_state);

                REQUIRE(!mesh_editor.in_selection(eh));
                REQUIRE(!mesh_editor.in_selection(v2h));

                REQUIRE(mesh.color(eh) == unselected_colour);
                REQUIRE(mesh.color(v2h) == unselected_colour);
            }
        }
    }
}
