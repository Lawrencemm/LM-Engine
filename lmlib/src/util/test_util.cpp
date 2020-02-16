#include <thread>

#include <catch2/catch.hpp>

#include <lmlib/util/range.h>
#include <lmlib/util/remote_executor.h>

TEST_CASE("Add lots of small tasks to a task queue")
{
    lm::remote_executor re;
    for (auto i : lm::range(100))
    {
        re.run([i] { ; });
    }
}
