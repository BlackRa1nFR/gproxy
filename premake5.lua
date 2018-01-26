workspace "gproxy"
    configurations {
        "Debug", "Release"
    }
    location "gproxy"

project "gproxy"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    characterset "MBCS"
    callingconvention "FastCall"
    architecture "x86"

    includedirs "headers"
    files { "headers/*.h", "src/*.c", "src/*.cpp", "headers/*.hpp" }

    vpaths {
        ["headers/*"] = "headers/*",
        ["src/*"] = "src/*"
    }

    configuration "Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Debug"
        targetdir "debug"

    configuration "Release"
        defines { "NDEBUG" }
        optimize "Full"
        targetdir "release"