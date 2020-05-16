from conans import ConanFile, CMake


class LmglConan(ConanFile):
    name = "lmgl"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    requires = (
        "lmlib/0.0.1",
        ("lmpl/0.0.1", 'private'),
        'Vulkan/1.1.92.1@lawrencem/stable',
    )
    build_requires = (
        'Catch2/2.5.0@catchorg/stable',
    )

    def source(self):
        self.run("git clone https://github.com/Lawrencemm/lmgl.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="lmgl", )
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*.h", dst="include", src="lmgl/include")

    def package_info(self):
        self.cpp_info.libs = ["lmgl"]
