#pragma once

#include <stdexcept>
#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <lmng/loaders/loader.h>

namespace lmng
{
class asset_cache
{
  public:
    explicit asset_cache(std::filesystem::path content_dir);

    template <typename loader_type, typename... arg_types>
    asset_cache &emplace_loader(arg_types &&... args)
    {
        using load_type = decltype(std::declval<loader_type>().load(*this, ""));
        using asset_type = typename load_type::element_type;
        loaders[entt::type_info<asset_type>::id()] =
          std::make_unique<loader_type>(std::forward<arg_types>(args)...);
        return *this;
    }

    template <typename asset_type>
    asset_loader_interface<asset_type> &get_loader()
    {
        using asset_type_info = entt::type_info<asset_type>;
        auto found = loaders.find(asset_type_info ::id());
        if (found == loaders.end())
        {
            throw std::runtime_error{("Loader not found for " + std::string{asset_type_info::name()}).c_str()};
        }
        return dynamic_cast<asset_loader_interface<asset_type> &>(*found->second);
    }

    template <typename asset_type>
    std::shared_ptr<asset_type> load(std::string const &asset_path)
    {
        return get_loader<asset_type>().load(*this, asset_path);
    }

    std::filesystem::path content_dir;

  protected:
    std::
      unordered_map<ENTT_ID_TYPE, std::unique_ptr<base_asset_loader_interface>>
        loaders;
};
} // namespace lmng
