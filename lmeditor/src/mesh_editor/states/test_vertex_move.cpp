#include <catch2/catch.hpp>

#include <lmtk/test.h>

#include "../test_mesh_editor.h"
#include "select.h"

using namespace lmeditor;

TEST_CASE_METHOD(MeshEditorTestCase, "Vertex Move Mode")
{
    lmtk::input_state input_state;

    auto vh = editable_mesh::vertex_handle{GENERATE(0, 1, 2, 3)};

    auto vpoint = mesh_editor.point(vh);
    lmtk::move_mouse(mesh_editor, vpoint, input_state);

    click(mesh_editor, vpoint, input_state);

    hit(mesh_editor, lmpl::key_code::M, input_state);

    SECTION("Vertex moves with mouse")
    {
        auto vdest = vpoint + lm::point2i{10, 10};

        lmtk::move_mouse(mesh_editor, vdest, input_state);

        vpoint = mesh_editor.point(vh);

        REQUIRE(vpoint == vdest);

        SECTION("Vertex remains at new position when leaving mode")
        {
            hit(mesh_editor, lmpl::key_code::M, input_state);

            REQUIRE(vpoint == vdest);
        }
    }
}
