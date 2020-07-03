#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include <lmng/meta/context.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    lmng::reflect_types();
    spdlog::set_level(spdlog::level::debug);

    int result = Catch::Session().run(argc, argv);

    return result;
}
