from conans import ConanFile, CMake


class LmhuvConan(ConanFile):
    name = "lmhuv"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    requires = (
        "lmengine/0.0.1",
        "lmgl/0.0.1",
        "lmtk/0.0.1",
        'OpenMesh/8.0@lawrencem/stable',
    )
    build_requires = (
        'embed-resource/0.2@lawrencem/stable',
    )
    default_options = {
        "OpenMesh:shared": True,
    }

    def imports(self):
        self.copy("embed-resource", src="bin")
        self.copy("embed-resource.exe*", src="bin")
        self.copy('embed-resource.cmake', dst='scripts', src='cmake')
        self.copy('glslangValidator*', src='bin')
        self.copy('*.dll', src='bin')

    def source(self):
        self.run("git clone https://gitlab.com/lm-engine/lmhuv.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="lmhuv")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*.h", dst="include", src="hello")
        self.copy("*lmhuv.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["lmhuv"]
