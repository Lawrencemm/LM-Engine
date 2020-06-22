#include <filesystem>
#include <iostream>

#include "app/app.h"
#include <clara.hpp>
#include <lmng/reflection.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv)
try
{
    auto project_dir = std::filesystem::current_path();
    auto cli = clara::Opt(project_dir, "directory")["--project-dir"](
      "Directory to save/load game files.");
    auto result = cli.parse(clara::Args(argc, argv));
    lmng::set_meta_context(entt::meta_ctx{});
    lmng::reflect_types();

    auto console_logger = spdlog::stdout_logger_mt("basic_logger");
    spdlog::set_default_logger(console_logger);

    lmeditor::editor_app app{project_dir};
    app.main();
}
catch (std::exception &e)
{
    std::cout << e.what() << std::endl;
}
