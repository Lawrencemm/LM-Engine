from conans import ConanFile, CMake, tools


class LmtkConan(ConanFile):
    name = "lmtk"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package", "virtualrunenv"
    requires = (
        'lmlib/0.0.1',
        "lmgl/0.0.1",
        "lmpl/0.0.1",
        ('scope_guard/0.2.3@lawrencem/stable', 'private'),
        ('embed-resource/0.1@lawrencem/stable', 'private'),
        ('freetype/2.10.0@bincrafters/stable', 'private'),
        'boost/1.70.0@conan/stable',
        'readerwriterqueue/1.0.1@lawrencem/stable',
    )

    def imports(self):
        self.copy("embed-resource", src="bin")
        self.copy("embed-resource.exe*", src="bin")
        self.copy('embed-resource.cmake', dst='scripts', src='cmake')
        self.copy('glslangValidator*', src='bin')

    def package_info(self):
        self.cpp_info.libs = ["lmtk"]
