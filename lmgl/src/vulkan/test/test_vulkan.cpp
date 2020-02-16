#include <iostream>

#include <catch2/catch.hpp>

#include <lmlib/pc_platform/test/mock_lmpl.h>

#include "../../../pc_platform/internal_platform_window.h"
#include "../vulkan_renderer.h"
#include "../vulkan_window_stage.h"

using namespace lmgl;

class MockWindowForVulkan : public MockWindow,
                            public IPlatformWindowInternal
{
  public:
    using MockWindow::MockWindow;
    vk::UniqueSurfaceKHR
      get_vk_surface(lmgl::vulkan_renderer &renderer) override
    {
        return vk::UniqueSurfaceKHR();
    }
};
