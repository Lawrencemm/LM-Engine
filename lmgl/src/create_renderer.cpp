#include "vulkan/vulkan_renderer.h"

namespace lmgl
{
renderer create_renderer(const renderer_init &init)
{
    auto ptr = std::make_unique<vulkan_renderer>(init);
    return ptr;
}
} // namespace lmgl
