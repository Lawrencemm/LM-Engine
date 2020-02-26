from conans import ConanFile, CMake, tools


class LmeditorConan(ConanFile):
    name = "lmeditor"
    version = "0.0.1"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Lmeditor here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package", "virtualrunenv"
    requires = (
        'lmengine/0.0.1',
        'lmgl/0.0.1',
        'lmlib/0.0.1',
        'lmpl/0.0.1',
        'lmtk/0.0.1',
        "lmhuv/0.0.1",
    )
    build_requires = (
        'OpenMesh/8.0@lawrencem/stable',
        'Catch2/2.6.1@catchorg/stable',
        'embed-resource/0.2@lawrencem/stable',
        'fmt/5.3.0@bincrafters/stable',
        'yaml-cpp/0.6.2@bincrafters/stable',
        'clara/1.1.5@bincrafters/stable',
        'boost/1.70.0',
    )

    def imports(self):
        self.copy("embed-resource", src="bin")
        self.copy("embed-resource.exe*", src="bin")
        self.copy('embed-resource.cmake', dst='scripts', src='cmake')
        self.copy('glslangValidator*', src='bin')
        self.copy('*.dll', src='bin')

    def package_info(self):
        self.cpp_info.libs = ['lmeditor']
