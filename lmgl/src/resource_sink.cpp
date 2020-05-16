#include <lmgl/renderer.h>
#include <lmgl/resource_sink.h>

namespace lmgl
{
void resource_store::free(irenderer *renderer)
{
    for (auto material : materials)
        renderer->destroy_material(material);
}
} // namespace lmgl
