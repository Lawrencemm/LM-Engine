#include <catch2/catch.hpp>

#include <lmtk/test.h>

#include "../test_mesh_editor.h"
#include "select.h"

using namespace lmeditor;

TEST_CASE_METHOD(MeshEditorTestCase, "Edge Move Mode")
{
    lmtk::input_state input_state;

    hit(mesh_editor, lmpl::key_code::E, input_state);

    auto eh = editable_mesh::edge_handle{GENERATE(0, 1, 2, 3)};

    auto epoint = mesh_editor.point(eh);

    click(mesh_editor, epoint, input_state);

    hit(mesh_editor, lmpl::key_code::M, input_state);

    SECTION("Edge moves with mouse")
    {
        auto edest = epoint + lm::point2i{10, 10};

        lmtk::move_mouse(mesh_editor, edest, input_state);

        epoint = mesh_editor.point(eh);

        REQUIRE(epoint == edest);

        SECTION("Edge remains at new position when leaving mode")
        {
            hit(mesh_editor, lmpl::key_code::M, input_state);

            REQUIRE(epoint == edest);
        }
    }
}
