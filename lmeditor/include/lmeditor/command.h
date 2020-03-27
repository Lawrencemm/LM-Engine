#pragma once

#include <string>
#include <vector>

namespace lmeditor
{
struct command_description
{
    std::string name, key, context;
};

using command_list = std::vector<command_description>;
} // namespace lmeditor
