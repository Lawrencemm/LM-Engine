#include <catch2/catch.hpp>

#include <model/trimesh.h>

TEST_CASE("Geometry mesh extrude edge")
{
    auto mesh = lmeditor::trimesh::xy_plane(0.f, 1.f);
    auto extruded_edge = mesh.extrude_edge(mesh.edge_handle(0));
}
