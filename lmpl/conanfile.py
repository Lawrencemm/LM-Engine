from conans import ConanFile, CMake
from conans.tools import os_info, SystemPackageTool, YumTool


class LmplConan(ConanFile):
    name = "lmpl"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    requires = (
        "lmlib/0.0.1",
        ('scope_guard/0.2.3@lawrencem/stable', 'private'),
    )
    build_requires = (
        'Catch2/2.5.0@catchorg/stable',
    )

    def source(self):
        self.run("git clone https://github.com/lawrencem99/lmpl.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="lmpl", )
        cmake.build()
        cmake.install()

    def system_requirements(self):
        if os_info.linux_distro == "fedora" or os_info.linux_distro == "centos":
            tool = SystemPackageTool(tool=YumTool(), conanfile=self)
            tool.install("gtkmm30-devel")
            tool.install("xcb-util-devel")

    def package(self):
        self.copy("*.h", dst="include", src="lmlib/include")

    def package_info(self):
        self.cpp_info.libs = ["lmpl"]
