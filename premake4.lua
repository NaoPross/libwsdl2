solution("wrapsdl2")
    -- build configurations
    configurations({ "debug", "release" })

    -- TODO: remove libmm relative path
    includedirs("../libmm/include")

    -- global build flags
    flags({ "ExtraWarnings" })
    buildoptions({ "-std=c++17" })

    -- global flags for g++
    configuration({ "linux", "gmake" })
        buildoptions({
            "-pedantic", "-Wall", "-Wextra", "-Wcast-qual", "-Wcast-align",
            "-Wpointer-arith", "-Winit-self", "-Wshadow", "-Wswitch-enum", 
            "-Wredundant-decls", "-Wfloat-equal", "-Wundef", "-Wvla", 
            "-Wconversion", "-Wstrict-aliasing"
        })

    -- libraries
    libdirs({".."})

    local libs = {
        sdl = os.findlib("SDL2"),
        mm = os.findlib("libmm"),
    }

    for k, v in pairs(libs) do
        io.write("library " ..k.. " ")
        if v ~= nil then
            print("found under " ..v)
        else
            print("not found!")
            os.exit(1)
        end
    end

    -- main project
    project("wsdl2")
        kind("StaticLib")
        language("C++")
        location("build")

        files("*.cpp")
        includedirs({"./", "include"})

        configuration("debug")
            targetdir("build/debug")
            defines("DEBUG")
            flags("Symbols")

        configuration("release")
            targetdir("build/release")
            flags("OptimizeSpeed")

    -- tests
    project("window-test")
        kind("ConsoleApp")
        language("C++")
        location("build")

        files("test/*.cpp")
        includedirs("include")

        links("wsdl2")

        configuration("debug")
            targetdir("build/debug")
            defines("DEBUG")
            flags("Symbols")

        configuration("release")
            targetdir("build/release")
            flags("OptimizeSpeed")
