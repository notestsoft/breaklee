from conan import ConanFile
from conan.tools.cmake import CMake

class BreakleeConan(ConanFile):
    name = "breaklee"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "libuv/1.48.0",
        "lua/5.4.7",
        "openssl/3.3.1",
        "zlib/1.3.1",
    ]
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["libuv"].shared = False
        self.options["lua"].shared = False
        self.options["openssl"].shared = False
        self.options["zlib"].shared = False

        if self.settings.os != "Windows":
            self.requires("odbc/2.3.11")
            self.options["odbc/2.3.11"].shared = False

    def layout(self):
        self.folders.build = "Build"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib", dst="bin", src="lib")
        self.copy("*.so", dst="bin", src="lib")
        self.copy("*.a", dst="lib", src="lib")
        self.copy("*.lib", dst="lib", src="lib")