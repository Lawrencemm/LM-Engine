#include <lmng/meta/clone.h>

namespace lmng
{
void clone(entt::registry &dst, entt::registry &src)
{
    dst.assign(src.data(), src.data() + src.size());

    entt::resolve([&](entt::meta_type const &component_type) {
        if (!component_type.prop(entt::hashed_string{"is_component"}))
            return;

        component_type.func("copy_pool"_hs)
          .invoke({}, std::reference_wrapper(dst), std::reference_wrapper(src));
    });
}
} // namespace lmng
