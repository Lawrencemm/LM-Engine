#include <tuple>

#include <Eigen/Eigen>

#include "editable_mesh.h"

namespace lmeditor
{
editable_mesh::editable_mesh(const editable_mesh_init &init)
    : geo_mesh{init.geo_mesh}, renderer{init.renderer}
{
    init_geometry(init);
    init_rendering(init);
}

} // namespace lmeditor
