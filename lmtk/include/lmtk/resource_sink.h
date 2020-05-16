#pragma once

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <lmgl/lmgl.h>
#include <map>
#include <vector>

namespace lmtk
{
struct resource_store
{
    std::vector<lmgl::buffer> buffers;
    std::vector<lmgl::geometry> geometries;
    std::vector<lmgl::material> materials;
    std::vector<lmgl::texture> textures;

    resource_store &add(lmgl::buffer &buffer)
    {
        buffers.emplace_back(std::move(buffer));
        return *this;
    }
    resource_store &add(lmgl::geometry &geometry)
    {
        geometries.emplace_back(std::move(geometry));
        return *this;
    }
    resource_store &add(lmgl::material material)
    {
        materials.emplace_back(material);
        return *this;
    }
    resource_store &add(lmgl::texture &texture)
    {
        textures.emplace_back(std::move(texture));
        return *this;
    }
};

class resource_sink
{
  public:
    void add_frame(lmgl::iframe *frame)
    {
        resource_map.emplace(std::pair{frame, resource_store{}});
    }

    void free_frame(lmgl::iframe *frame, lmgl::irenderer *renderer)
    {
        auto foundit = resource_map.find(frame);
        if (foundit == resource_map.end())
            return;

        auto &store = foundit->second;
        for (auto material : store.materials)
            renderer->destroy_material(material);
        resource_map.erase(frame);
    }

    template <typename resource_type>
    resource_sink &add(lmgl::irenderer *renderer, resource_type &resource)
    {
        if (resource_map.empty())
        {
            resource.reset();
            return *this;
        }
        resource_map.rbegin()->second.add(resource);
        return *this;
    }

    template <typename... resource_types>
    resource_sink &
      add(lmgl::irenderer *renderer, resource_types &&... resources)
    {
        auto tup = std::tie(resources...);
        boost::fusion::for_each(
          tup, [&](auto &resource) { add(renderer, resource); });
        return *this;
    }

  private:
    std::map<lmgl::iframe *, resource_store> resource_map;
};

template <>
resource_sink &resource_sink::add<lmgl::material>(
  lmgl::irenderer *renderer,
  lmgl::material &resource);
} // namespace lmtk
