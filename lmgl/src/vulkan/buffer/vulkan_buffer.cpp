#include <buffer.h>
#include <lmlib/variant_visitor.h>

#include "../vulkan_renderer.h"
#include "vulkan_buffer.h"

namespace lmgl
{

buffer vulkan_renderer::create_buffer(const buffer_init &init)
{
    return std::make_unique<vulkan_buffer>(init, this);
}

vulkan_buffer::vulkan_buffer(
  const buffer_init &init,
  vulkan_renderer *renderer_in)
    : vulkan_buffer_base{init, renderer_in},
      buffer{create_buffer(array_size(init.data))},
      memory{create_memory(buffer)}
{
    renderer->vk_device->bindBufferMemory(*buffer, *memory, 0);
    if (init.data.first_element_addr)
        update_memory(memory.get(), init.data);
}

void *vulkan_buffer::map(size_t offset, size_t size)
{
    return renderer->vk_device->mapMemory(memory.get(), offset, size);
}

void vulkan_buffer::unmap() { renderer->vk_device->unmapMemory(memory.get()); }

} // namespace lmgl
