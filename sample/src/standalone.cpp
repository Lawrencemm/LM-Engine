#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>
#include <SDL_messagebox.h>

#include "application.h"
#include "reflect_types.h"

#include <lmng/meta/context.h>

int main()
try
{
    lmng::set_meta_context(entt::meta_ctx{});
    lmng::reflect_types();
    reflect_types_cpp();
    sample_app app{};
    app.main();
    return 0;
}
catch (std::exception &e)
{
    std::cout << e.what() << std::endl;

    SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR, "Application Crashed", e.what(), nullptr);
    return 1;
}
