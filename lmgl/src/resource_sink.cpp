#include <lmgl/renderer.h>
#include <lmgl/resource_sink.h>

namespace lmgl
{
void resource_store::free(irenderer *renderer)
{
    for (auto material : materials)
        renderer->destroy_material(material);

    materials.clear();
    buffers.clear();
    geometries.clear();
    textures.clear();
}
} // namespace lmgl
