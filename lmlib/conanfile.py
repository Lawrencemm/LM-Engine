from conans import ConanFile, CMake


class LmlibConan(ConanFile):
    name = "lmlib"
    version = "0.0.1"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Lmlib here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    default_options = {"TBB:shared": True, "fmt:header_only": True}
    generators = "cmake_find_package", 'virtualenv'
    requires = (
        'eigen/3.3.5@conan/stable',
        'TBB/2019_U4@conan/stable',
        'range-v3/0.9.1',
        ('scope_guard/0.2.3@lawrencem/stable', 'private'),
        ('Vulkan/1.1.92.1@lawrencem/stable', 'private'),
        'fmt/5.3.0@bincrafters/stable',
    )
    build_requires = (
        'Catch2/2.5.0@catchorg/stable',
    )

    def source(self):
        self.run("git clone https://github.com/lawrencem99/lmlib.git")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="lmlib", )
        self.run('. ./activate.sh')

        cmake.build(
            args=["-DCMAKE_BUILD_SHARED_LIBS"] if self.options.shared else [])
        cmake.install()

    def package(self):
        self.copy("*.h", dst="include", src="lmlib/include")

    def package_info(self):
        self.cpp_info.libs = ["lmlib"]
