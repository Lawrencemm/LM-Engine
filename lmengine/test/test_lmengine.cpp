#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include <lmengine/reflection.h>

int main(int argc, char *argv[])
{
    lmng::reflect_types();

    int result = Catch::Session().run(argc, argv);

    return result;
}
