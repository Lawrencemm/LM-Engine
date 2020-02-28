from conans import ConanFile, CMake, tools


class LmengineConan(ConanFile):
    name = "lmengine"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    requires = (
        'lmlib/0.0.1',
        'cereal/1.2.2@conan/stable',
        'entt/3.3.0',
        'bullet3/2.89',
        'yaml-cpp/0.6.2@bincrafters/stable',
    )
    build_requires = (
        'Catch2/2.6.1@catchorg/stable',
    )

    def imports(self):
        self.copy('*.dll', src='bin')

    def package_info(self):
        self.cpp_info.libs = ['lmengine']
