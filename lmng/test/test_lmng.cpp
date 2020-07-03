#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include <lmng/meta/context.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    lmng::reflect_types();

    int result = Catch::Session().run(argc, argv);

    return result;
}
