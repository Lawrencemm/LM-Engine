#pragma once

#include <entt/core/hashed_string.hpp>
#include <lmng/asset_cache.h>

namespace std
{
template <> struct hash<entt::hashed_string>
{
    std::size_t operator()(entt::hashed_string const &s) const noexcept
    {
        return s.value();
    }
};
} // namespace std

class test_asset_cache;

template <typename asset_type>
class test_asset_loader : public lmng::asset_loader_interface<asset_type>
{
  public:
    friend class fake_asset_cache;

    std::shared_ptr<asset_type>
      load(lmng::asset_cache &cache, std::string const &asset_path)
    {
        return data.at(entt::hashed_string{asset_path.c_str()});
    }

    template <typename... constructor_arg_types>
    void emplace(
      std::string const &asset_path,
      constructor_arg_types &&... constructor_args)
    {
        data[entt::hashed_string{asset_path.c_str()}] =
          std::make_shared<asset_type>(
            std::forward<constructor_arg_types>(constructor_args)...);
    }

    std::unordered_map<entt::hashed_string, std::shared_ptr<asset_type>> data;
};

class test_asset_cache : public lmng::asset_cache
{
  public:
    template <typename asset_type, typename... constructor_arg_types>
    void emplace(
      std::string const &asset_path,
      constructor_arg_types &&... constructor_args)
    {
        if (loaders.find(entt::type_info<asset_type>::id()) == loaders.end())
        {
            emplace_loader<test_asset_loader<asset_type>>();
        }

        dynamic_cast<test_asset_loader<asset_type> &>(get_loader<asset_type>())
          .emplace(
            asset_path,
            std::forward<constructor_arg_types>(constructor_args)...);
    }
};
