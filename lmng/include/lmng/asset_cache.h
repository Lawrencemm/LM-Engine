#pragma once

#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace lmng
{
class base_asset_loader_interface
{
  public:
    virtual ~base_asset_loader_interface() = default;
};

class asset_cache;

template <typename asset_type>
class asset_loader_interface : public base_asset_loader_interface
{
  public:
    virtual std::shared_ptr<asset_type>
      load(asset_cache &cache, std::string const &asset_path) = 0;
};

class asset_cache
{
  public:
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
        return dynamic_cast<asset_loader_interface<asset_type> &>(
          *loaders[entt::type_info<asset_type>::id()]);
    }

    template <typename asset_type>
    std::shared_ptr<asset_type> load(std::string const &asset_path)
    {
        return get_loader<asset_type>().load(*this, asset_path);
    }

  protected:
    std::
      unordered_map<ENTT_ID_TYPE, std::unique_ptr<base_asset_loader_interface>>
        loaders;
};
} // namespace lmng
