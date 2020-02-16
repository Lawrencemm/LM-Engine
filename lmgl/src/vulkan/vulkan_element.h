#pragma once
#include <vector>

#include <element.h>

#include "vulkan_frame.h"

namespace lmgl
{
class vulkan_scene;
class vulkan_frame;
class vulkan_element;
class vulkan_frame_element;

class vulkan_element : public virtual ielement
{
  public:
    vulkan_element(vulkan_renderer *renderer) : renderer{renderer} {}
    virtual lm::reference<vulkan_frame_element>
      create_context_node(vk::Viewport const &viewport) const = 0;

  public:
    vulkan_renderer *renderer;
};

class vulkan_frame_element
{
  public:
    virtual void render(vulkan_frame &context) = 0;
    virtual ~vulkan_frame_element() = default;
};
} // namespace lmgl
