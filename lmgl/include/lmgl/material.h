#pragma once

#include <lmlib/array_proxy.h>

namespace lmgl
{
enum class index_type
{
    two_bytes,
    four_bytes
};
enum class topology
{
    point_list,
    line_list,
    triangle_list
};
enum class input_type
{
    float_2,
    float_3,
    float_4,
    mat_4
};
enum class vertex_input_rate
{
    vertex,
    instance
};
struct vertex_binding
{
    size_t size;
    vertex_input_rate input_rate{vertex_input_rate::vertex};
};
struct vertex_input
{
    input_type type;
    uint32_t binding, offset{0};
};
enum class polygon_mode
{
    fill,
    line
};

struct material_init
{
    lm::array_proxy<char const> vshader_spirv;
    lm::array_proxy<char const> pshader_spirv{nullptr};
    lmgl::index_type index_type;
    lmgl::topology topology{lmgl::topology::triangle_list};
    lm::array_proxy<vertex_binding const> const vertex_bindings;
    lm::array_proxy<vertex_input const> const vertex_inputs;
    size_t uniform_size;
    bool texture{false};
    bool do_depth_test{true};
    bool write_stencil{false};
    bool test_stencil{false};
    lmgl::polygon_mode polygon_mode{lmgl::polygon_mode::fill};
};
} // namespace lmgl
