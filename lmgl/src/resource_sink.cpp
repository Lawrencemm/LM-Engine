#include <lmgl/resource_sink.h>

namespace lmgl
{
template <>
resource_sink &
  resource_sink::add<material>(irenderer *renderer, material &resource)
{
    if (resource_map.empty())
    {
        renderer->destroy_material(resource);
        return *this;
    }
    resource_map.rbegin()->second.add(resource);
    return *this;
}
} // namespace lmgl
