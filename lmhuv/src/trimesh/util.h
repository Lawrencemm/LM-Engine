#pragma once

#include <lmgl/lmgl.h>
#include <lmhuv/trimesh.h>

namespace lmhuv
{
inline lmgl::index_type get_mesh_index_type()
{
    constexpr auto size = sizeof(lmhuv::trimesh::VertexHandle);
    static_assert(
      (size == 4) || (size == 2), "Mesh vertex handle is not 16 or 32 bits.");
    if constexpr (size == 4)
        return lmgl::index_type::four_bytes;
    if constexpr (size == 2)
        return lmgl::index_type::two_bytes;
}
} // namespace lmhuv
