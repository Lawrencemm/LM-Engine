#include "application.h"
#include "reflection.h"

#include <lmng/reflection.h>

int main()
{
    lmng::set_meta_context(entt::meta_ctx{});
    lmng::reflect_types();
    reflect_types_cpp();
    sample_app app{};
    app.main();
    return 0;
}
