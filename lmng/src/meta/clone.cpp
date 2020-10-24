#include <lmng/meta/clone.h>

namespace lmng
{
void clone(entt::registry &dst, entt::registry const &src)
{
    dst.assign(src.data(), src.data() + src.size());

    for (auto const &component_type : entt::resolve())
    {
        if (!component_type.prop(entt::hashed_string{"is_component"}))
            return;

        component_type.func("copy_pool"_hs)
          .invoke(
            {},
            std::reference_wrapper(dst),
            std::reference_wrapper(const_cast<entt::registry &>(src)));
    };
}
} // namespace lmng
