#pragma once

#include <lmgl/lmgl.h>
#include <map>
#include <vector>

namespace lmgl
{
struct resource_store
{
    std::vector<buffer> buffers;
    std::vector<geometry> geometries;
    std::vector<material> materials;
    std::vector<texture> textures;

    resource_store &add(buffer &buffer)
    {
        buffers.emplace_back(std::move(buffer));
        return *this;
    }
    resource_store &add(geometry &geometry)
    {
        geometries.emplace_back(std::move(geometry));
        return *this;
    }
    resource_store &add(material material)
    {
        materials.emplace_back(material);
        return *this;
    }
    resource_store &add(texture &texture)
    {
        textures.emplace_back(std::move(texture));
        return *this;
    }

    void free(irenderer *renderer);
};

class resource_sink
{
  public:
    void add_frame(iframe *frame)
    {
        resource_map.emplace(std::pair{frame, resource_store{}});
    }

    void free_frame(iframe *frame, irenderer *renderer)
    {
        orphaned_resources.free(renderer);

        auto foundit = resource_map.find(frame);
        if (foundit == resource_map.end())
            return;

        foundit->second.free(renderer);
        resource_map.erase(frame);
    }

    template <typename resource_type>
    resource_sink &add(resource_type &resource)
    {
        if (resource_map.empty())
        {
            orphaned_resources.add(resource);
            return *this;
        }
        resource_map.rbegin()->second.add(resource);
        return *this;
    }

    template <typename... resource_types>
    resource_sink &add(resource_types &&... resources)
    {
        (add(resources), ...);
        return *this;
    }

  private:
    resource_store orphaned_resources;
    std::map<iframe *, resource_store> resource_map;
};
} // namespace lmgl
