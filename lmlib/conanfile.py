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
    default_options = {"TBB:shared": True, "fmt:header_only": False, "spdlog:shared": True}
    generators = "cmake_find_package", 'virtualenv'
    requires = (
        'eigen/3.3.5@conan/stable',
        'tbb/2020.1',
        'range-v3/0.10.0@lawrencemm/stable',
        'spdlog/1.6.1',
        ('scope_guard/0.2.3@lawrencem/stable', 'private'),
        ('Vulkan/1.1.92.1@lawrencem/stable', 'private'),
        'fmt/6.2.1',
        'boost/1.73.0',
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

    def imports(self):
        if (not self.in_local_cache):
            self.copy("*.dll", src="bin", dst="../..")
