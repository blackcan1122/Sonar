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
        print("Unzipping to " ..  os.getcwd())
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
        print("Unzipping to " ..  os.getcwd())
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
        print("Unzipping to " ..  os.getcwd())
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

-- This is necessary, otherwise compilation will fail since
-- there is no CLOCK_MONOTOMIC. raylib claims to have a workaround
-- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
-- to work. raylib's Makefile also adds this flag, probably why it went
-- unnoticed for so long.
-- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
-- to have CLOCK_MONOTOMIC
-- See: https://github.com/raysan5/raylib/issues/2729

    filter{}
end

-- if you don't want to download raylib, then set this to false, and set the raylib dir to where you want raylib to be pulled from, must be full sources.
downloadRaylib = true
raylib_dir = "external/raylib-master"
spdlog_dir = "external/spdlog-1.x"
json_dir = "external/nlohmann"

workspaceName = 'Sonar'
baseName = path.getbasename(path.getdirectory(os.getcwd()));

--if (baseName ~= 'raylib-quickstart') then
    workspaceName = baseName
--end

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
        "../src/shaders/**.vs",
        "../src/shaders/**.fs"
    }

    filter { "files:**.vs", "files:**.fs", "action:vs*" }
      buildaction           "Content"
    filter {}  -- clear filter

    defaultplatform ("x64")

    -- Apply global settings for Linux
    filter { "system:linux" }
        architecture "x86_64"       -- 64‑bit Linux only runners 
        pic          "On"           -- PIC for static libs on x86_64 :contentReference[oaicite:4]{index=4}
        defines      { "_GLFW_X11", "_GNU_SOURCE" }  -- Enable X11 and GNU extensions :contentReference[oaicite:5]{index=5}
        links        { "pthread", "m", "dl", "rt", "X11" }  -- System libs for raylib on Linux :contentReference[oaicite:6]{index=6}

    filter {}  -- Clear filters so subsequent settings apply globally

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
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

    startproject(workspaceName)

    project (workspaceName)
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
            ["Header Files/*"] = { "../include/**.h",  "../include/**.hpp", "../src/**.h", "../src/**.hpp"},
            ["Source Files/*"] = {"../src/**.c", "src/**.cpp"},
        }
        files {"../src/**.c", "../src/**.cpp", "../src/**.h", "../src/**.hpp", "../include/**.h", "../include/**.hpp"}
    
        includedirs { "../src" }
        includedirs { "../src/public" }
        includedirs { "../src/private" }
        includedirs { "../include" }

        links {"raylib"}

        cdialect "C99"
        cppdialect "C++23"

        includedirs {raylib_dir .. "/src" }
        includedirs {raylib_dir .."/src/external" }
        includedirs { raylib_dir .."/src/external/glfw/include" }

        includedirs {spdlog_dir .. "/include" }
        includedirs {json_dir}
        
        defines { "SPDLOG_COMPILED_LIB" }


        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"raylib", "spdlog", "nlohmann"}
            links {"raylib.lib", "spdlog.lib"}
            libdirs { "../bin/%{cfg.buildcfg}" }  -- Tell linker where to find .lib files
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus", "/utf-8", "/openmp", "/bigobj" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            libdirs {"../bin/%{cfg.buildcfg}"}


        filter "system:linux"
            links {"spdlog", "pthread", "m", "dl", "rt", "X11"}
            kind   "ConsoleApp"
            buildoptions { "-fopenmp" }
            linkoptions { "-fopenmp" }

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}

        postbuildcommands {
            -- Cross-platform way to copy entire folder recursively
            '{COPYDIR} "../../resources/" "%{cfg.targetdir}/resources"',
            '{MKDIR} "%{cfg.targetdir}/src"',
            '{COPYDIR} "../../src/shaders" "%{cfg.targetdir}/src/shaders"'

        }
		

    project "raylib"
        kind "StaticLib"
    
        platform_defines()

        location "build_files/"

        language "C"
        targetdir "../bin/%{cfg.buildcfg}"

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
        targetdir "../bin/%{cfg.buildcfg}"  -- Match raylib's output directory
    
        -- Include spdlog headers and source files
        includedirs { spdlog_dir .. "/include" }
        files { 
            spdlog_dir .. "/include/**.h", 
            spdlog_dir .. "/src/**.cpp"  -- Include all .cpp files recursively
        }
    
        -- Required for static library compilation
        defines { "SPDLOG_COMPILED_LIB" }
    
        -- Platform-specific configurations
        filter { "system:windows" }
            defines { "_CRT_SECURE_NO_WARNINGS" }
            characterset "Unicode"
    
        filter { "configurations:Debug" }
            symbols "On"
    
        filter { "configurations:Release" }
            optimize "On"
        
        filter "action:vs*"
            buildoptions { "/Zc:__cplusplus", "/utf-8" }

        filter { "system:linux" }
            defines { "PLATFORM_LINUX" }
        
        filter { "system:windows" }
            defines { "PLATFORM_WINDOWS" }


    project "nlohmann"
        kind "none"
        language "C++"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"  -- Match raylib's output directory
    
        -- Include spdlog headers and source files
        includedirs { json_dir }
        files { 
            json_dir .. "/**.hpp",
            json_dir .. "/include/single_include/nlohmann/**.hpp"
        }
    
        -- Required for static library compilation
        defines { "Json_LIB" }
    
        -- Platform-specific configurations
        filter { "system:windows" }
            defines { "_CRT_SECURE_NO_WARNINGS" }
            characterset "Unicode"
    
        filter { "configurations:Debug" }
            symbols "On"
    
        filter { "configurations:Release" }
            optimize "On"
        
        filter "action:vs*"
            buildoptions { "/Zc:__cplusplus", "/utf-8" }
