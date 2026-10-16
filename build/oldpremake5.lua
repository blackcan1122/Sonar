newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"},
        { "openges2", "OpenGL ES2"},
        { "openges3", "OpenGL ES3"}
    },
    default = "opengl33"
}

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    os.chdir("external")
    if(os.isdir("raylib-master") == false) then
        if(not os.isfile("raylib-master.zip")) then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " .. os.getcwd())
        zip.extract("raylib-master.zip", os.getcwd())
        os.remove("raylib-master.zip")
    end
    os.chdir("../")
end

function checkspdlog()
    os.chdir("external")
    if(os.isdir("spdlog-1.x") == false) then
        if(not os.isfile("v1.x.zip")) then
            print("spdlog not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/gabime/spdlog/archive/refs/heads/v1.x.zip", "v1.x.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " .. os.getcwd())
        zip.extract("v1.x.zip", os.getcwd())
        os.remove("v1.x.zip")
    end
    os.chdir("../")
end

function CheckNlohmann()
    os.chdir("external")
    if(os.isdir("nlohmann") == false) then
        if(not os.isfile("include.zip")) then
            print("NLohmann not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip", "include.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " .. os.getcwd())
        zip.extract("include.zip", os.getcwd() .. "/nlohmann")
        os.remove("include.zip")
    end
    os.chdir("../")
end

function build_externals()
    print("calling externals")
    check_raylib()
    checkspdlog()
    CheckNlohmann()
end

function platform_defines()
    filter {"configurations:Debug or Release"}
        defines{"PLATFORM_DESKTOP"}

    filter {"configurations:Debug_RGFW or Release_RGFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}

    filter{}
end

downloadRaylib = true
raylib_dir = "external/raylib-master"
spdlog_dir = "external/spdlog-1.x"
json_dir = "external/nlohmann"

engine_public_headers = {
    "../src/SONARENGINE/public/**.h",
    "../src/SONARENGINE/public/**.hpp"
}

engine_source_files = {
    "../src/SONARENGINE/private/**.cpp"
}

game_public_headers = {
    "../src/SONAR/public/**.h",
    "../src/SONAR/public/**.hpp"
}

game_source_files = {
    "../src/SONAR/main.cpp",
    "../src/SONAR/private/**.cpp"
}

workspaceName = 'Sonar'
baseName = path.getbasename(path.getdirectory(os.getcwd()))
workspaceName = baseName

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

if (os.isdir('external') == false) then
    os.mkdir('external')
end

workspace (workspaceName)
    location "../"
    configurations { "Debug", "Release", "Debug_RGFW", "Release_RGFW"}
    platforms { "x64", "x86", "ARM64"}

    files {
        "../src/SONAR/shaders/**.vs",
        "../src/SONAR/shaders/**.fs"
    }

    filter { "files:**.vs", "files:**.fs", "action:vs*" }
        buildaction "Content"
    filter {}

    defaultplatform ("x64")

    -- CRT consistency across ALL projects — fixes std::string size mismatch
    filter "action:vs*"
        toolset "v143"
        staticruntime "off"
    filter "configurations:Debug or Debug_RGFW"
        runtime "Debug"
    filter "configurations:Release or Release_RGFW"
        runtime "Release"
    filter {}

    filter { "system:linux" }
        architecture "x86_64"
        pic "On"
        defines { "_GLFW_X11", "_GNU_SOURCE" }
        links { "pthread", "m", "dl", "rt", "X11" }
    filter {}

    filter "configurations:Debug or Debug_RGFW"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release or Release_RGFW"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

if (downloadRaylib) then
    build_externals()
end

startproject("SONAR")

    project "SONARENGINE"
        kind "StaticLib"
        language "C++"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        vpaths
        {
            ["Header Files/*"] = engine_public_headers,
            ["Source Files/*"] = engine_source_files,
        }
        files(engine_public_headers)
        files(engine_source_files)
        files { "../include/**.h", "../include/**.hpp" }

        includedirs { "../src/SONARENGINE" }
        includedirs { "../src/SONARENGINE/public" }
        includedirs { "../src/SONARENGINE/private" }
        includedirs { "../src/SONAR/public" }
        includedirs { "../include" }
        includedirs { raylib_dir .. "/src" }
        includedirs { raylib_dir .. "/src/external" }
        includedirs { raylib_dir .. "/src/external/glfw/include" }
        includedirs { spdlog_dir .. "/include" }
        includedirs { json_dir }

        cdialect "C99"
        cppdialect "C++20"
        defines { "SPDLOG_COMPILED_LIB" }
        flags { "ShadowedVariables" }
        platform_defines()

        filter "action:vs*"
            defines { "_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS" }
            links { "raylib.lib", "spdlog.lib" }
            libdirs { "../bin/%{cfg.buildcfg}" }
            characterset("Unicode")
            buildoptions { "/Zc:__cplusplus", "/utf-8", "/openmp", "/bigobj", "/MP" }

        filter "system:windows"
            defines { "_WIN32" }
            links { "winmm", "gdi32", "opengl32", "dbghelp" }
            libdirs { "../bin/%{cfg.buildcfg}" }

        filter "system:linux"
            links { "raylib", "spdlog", "pthread", "m", "dl", "rt", "X11" }
            buildoptions { "-fopenmp" }
            linkoptions { "-fopenmp" }

        filter "system:macosx"
            links { "OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework" }

        filter{}

    project "SONAR"
        kind "ConsoleApp"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        filter {"system:windows", "configurations:Release", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }

        filter {"system:windows", "configurations:Release", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths
        {
            ["Header Files/*"] = game_public_headers,
            ["Source Files/*"] = game_source_files,
        }
        files(game_public_headers)
        files(game_source_files)
        files { "../include/**.h", "../include/**.hpp" }

        includedirs { "../src/SONAR" }
        includedirs { "../src/SONAR/public" }
        includedirs { "../src/SONAR/private" }

        -- SONARENGINE include directory
        includedirs { "../src/SONARENGINE/public" }

        includedirs { raylib_dir .. "/src" }
        includedirs { raylib_dir .. "/src/external" }
        includedirs { raylib_dir .. "/src/external/glfw/include" }
        includedirs { spdlog_dir .. "/include" }
        includedirs { json_dir }

        -- SONARENGINE library
        links { "SONARENGINE" }

        cdialect "C99"
        cppdialect "C++20"

        includedirs {spdlog_dir .. "/include" }
        includedirs {json_dir}

        defines { "SPDLOG_COMPILED_LIB" }

        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"SONARENGINE"}
            libdirs { "../bin/%{cfg.buildcfg}" }
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus", "/utf-8", "/openmp", "/bigobj", "/MP" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32", "dbghelp"}
            libdirs {"../bin/%{cfg.buildcfg}"}

        filter "system:linux"
            links {"SONARENGINE", "pthread", "m", "dl", "rt", "X11"}
            kind "ConsoleApp"
            buildoptions { "-fopenmp" }
            linkoptions { "-fopenmp" }

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}

        postbuildcommands {
            '{COPYDIR} "../../resources/" "%{cfg.targetdir}/resources"',
            '{MKDIR} "%{cfg.targetdir}/src"',
            '{COPYDIR} "../../src/SONAR/shaders" "%{cfg.targetdir}/src/shaders"'
        }


    project "raylib"
        kind "StaticLib"
        language "C"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {raylib_dir .. "/src", raylib_dir .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files"] = { raylib_dir .. "/src/**.h"},
            ["Source Files/*"] = { raylib_dir .. "/src/**.c"},
        }
        files {raylib_dir .. "/src/*.h", raylib_dir .. "/src/*.c"}
        removefiles {raylib_dir .. "/src/rcore_*.c"}

        filter { "system:macosx", "files:" .. raylib_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter { "system:linux" }
            pic "On"

        filter{}


    project "spdlog"
        kind "StaticLib"
        language "C++"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        includedirs { spdlog_dir .. "/include" }
        files {
            spdlog_dir .. "/include/**.h",
            spdlog_dir .. "/src/**.cpp"
        }

        defines { "SPDLOG_COMPILED_LIB" }

        filter "action:vs*"
            defines{"_CRT_SECURE_NO_WARNINGS"}
            characterset "Unicode"
            buildoptions { "/Zc:__cplusplus", "/utf-8" }

        filter { "system:linux" }
            defines { "PLATFORM_LINUX" }

        filter { "system:windows" }
            defines { "PLATFORM_WINDOWS" }

        filter{}


    project "nlohmann"
        kind "none"
        language "C++"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        includedirs { json_dir }
        files {
            json_dir .. "/**.hpp",
            json_dir .. "/include/single_include/nlohmann/**.hpp"
        }

        defines { "Json_LIB" }

        filter "action:vs*"
            defines{"_CRT_SECURE_NO_WARNINGS"}
            characterset "Unicode"
            buildoptions { "/Zc:__cplusplus", "/utf-8" }

        filter{}
