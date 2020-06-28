#include <catch2/catch.hpp>
#include <lmlib/intersection.h>

TEST_CASE("Intersection")
{
    CHECK(lm::distance({1.f, 0.f}, {2.f, 5.f}) == -1.f);
    CHECK(lm::distance({2.f, 0.f}, {2.f, 5.f}) == 0.f);
    CHECK(lm::distance({3.f, 0.f}, {2.f, 5.f}) == 1.f);
}
