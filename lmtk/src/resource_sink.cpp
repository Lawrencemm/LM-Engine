#include <lmtk/resource_sink.h>

namespace lmtk
{
template <>
resource_sink &resource_sink::add<lmgl::material>(
  lmgl::irenderer *renderer,
  lmgl::material &resource)
{
    if (resource_map.empty())
    {
        renderer->destroy_material(resource);
        return *this;
    }
    resource_map.rbegin()->second.add(resource);
    return *this;
}
} // namespace lmtk
