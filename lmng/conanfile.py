import os

from conans import ConanFile, CMake, tools


class lmngConan(ConanFile):
    name = "lmng"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    default_options = {
        "yaml-cpp:shared": True,
    }
    requires = (
        'lmlib/0.0.1',
        'cereal/1.2.2@conan/stable',
        'entt/3.3.0',
        'bullet3/2.89',
        'yaml-cpp/0.6.3',
    )
    build_requires = (
        'Catch2/2.6.1@catchorg/stable',
    )

    def imports(self):
        self.copy('*.dll', src='bin')
        if not self.in_local_cache and self.settings.os == "Windows":
            self.copy("*.dll", src="bin", dst=os.getenv("WORKSPACE_DIR"))

    def package_info(self):
        self.cpp_info.libs = ['lmng']
