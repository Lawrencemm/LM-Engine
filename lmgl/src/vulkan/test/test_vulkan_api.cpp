/// Tests for direct linking with the real Vulkan library.
/** Usually we will mock the API but these tests can be used to test with an
 * actual driver.
 */
#include <catch2/catch.hpp>

#include <lmlib/graphics/render_buffer.h>

#include "../vulkan_renderer.h"

using namespace lmgl;

TEST_CASE("Vulkan renderer")
{
    reference<event_queue> eq = std::make_shared<event_queue>();
    vulkan_renderer renderer{{eq}};
    SECTION("Two Vulkan renderers simultaneously")
    {
        vulkan_renderer renderer_2{{eq}};
    }
}
