from conans import ConanFile, CMake, tools


class SampleConan(ConanFile):
    name = "sample"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    requires = (
        'lmlib/0.0.1',
        "lmengine/0.0.1",
        "lmeditor/0.0.1",
        "lmtk/0.0.1",
        "lmhuv/0.0.1",
        'boost/1.70.0',
    )
    build_requires = (
        'Catch2/2.6.1@catchorg/stable',
    )
