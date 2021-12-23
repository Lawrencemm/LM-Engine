#pragma once
#include <memory>

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
}
