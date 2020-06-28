#include "application.h"
#include "reflect_types.h"

#include <lmng/meta/context.h>

int main()
{
    lmng::set_meta_context(entt::meta_ctx{});
    lmng::reflect_types();
    reflect_types_cpp();
    sample_app app{};
    app.main();
    return 0;
}
