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
        pthreads = os.findlib("pthread"),
        -- mm = os.findlib("mm"),
    }

    for k, v in pairs(libs) do
        io.write("library " ..k.. " ")
        if v == nil then
            print("not found!")
            os.exit(1)
        else
            print("found under " ..v)
        end
    end

    -- main project
    project("wsdl2")
        kind("StaticLib")
        language("C++")
        location("build")

        files("*.cpp")
        includedirs({"./", "include"})

        links({ "SDL2" })

        configuration("debug")
            targetdir("build/debug")
            defines({ "DEBUG", "WSDL2_USE_MM" })
            flags("Symbols")

        configuration("release")
            targetdir("build/release")
            defines({ "WSDL2_USE_MM" })
            flags("OptimizeSpeed")

    -- tests
    project("wsdl2-test-window")
        kind("ConsoleApp")
        language("C++")
        location("build")

        files("test/window_test.cpp")
        includedirs("include")

        links({ "wsdl2", "SDL2" })

        configuration("debug")
            targetdir("build/debug")
            defines("DEBUG")
            flags("Symbols")

        configuration("release")
            targetdir("build/release")
            flags("OptimizeSpeed")

    project("wsdl2-test-threaded-window")
        kind("ConsoleApp")
        language("C++")
        location("build")

        files("test/threaded_window_test.cpp")
        includedirs("include")

        links({ "wsdl2", "SDL2", "pthread" })

        configuration("debug")
            targetdir("build/debug")
            defines("DEBUG")
            flags("Symbols")

        configuration("release")
            targetdir("build/release")
            flags("OptimizeSpeed")
