# Building SONAR and the engine SDK

Use CMake 3.28 or newer and a C++20 compiler with `std::format` support. The tested Windows toolchains are Visual Studio 2022 and MSYS2 UCRT64 GCC. Linux CI uses GCC 14.

```powershell
cmake --preset msvc
cmake --build --preset msvc-debug
ctest --test-dir out/msvc -C Debug --output-on-failure

cmake --preset mingw-debug
cmake --build --preset mingw-debug
ctest --test-dir out/mingw-debug --output-on-failure
```

The MinGW presets expect MSYS2 at `C:/msys64/ucrt64`. Override compiler paths with a personal `CMakeUserPresets.json` or a direct CMake configure command when installed elsewhere. Ninja and the compiler's runtime DLL directory must be on `PATH` during configuration/build.

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
xvfb-run -a ctest --test-dir out/linux-debug --output-on-failure
```

Linux needs the X11 and OpenGL development packages listed in the Linux CI workflow. Use `linux-release`, `mingw-release`, or `msvc-release` for Release builds. MSVC shares one configure preset between configurations.

The source checkout contains no generated solution or build tree. CMake creates all build files and downloaded dependencies under `out/`; install SDKs under `dist/`. Both directories are disposable and ignored by Git. The old Premake `build/` and root `bin/` layout is retired.

CMake downloads raylib 5.5, spdlog 1.15.3, and nlohmann/json 3.12.0. To use existing source checkouts, configure with `SONAR_RAYLIB_SOURCE`, `SONAR_SPDLOG_SOURCE`, and `SONAR_JSON_SOURCE`. The JSON override may point at a checkout or the extracted include archive. These dependencies are private to SONARENGINE. `SONAR_RGFW=ON` selects the alternate desktop backend; GLFW is the default.

## Install and consume the SDK

```powershell
cmake --install out/mingw-debug --prefix dist/mingw-debug
cmake -S dist/mingw-debug/examples/sdk-consumer -B out/consumer -G Ninja -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/path/to/Sonar/dist/mingw-debug"
cmake --build out/consumer
$env:PATH = "C:/path/to/Sonar/dist/mingw-debug/bin;$env:PATH"
ctest --test-dir out/consumer --output-on-failure
```

For MSVC, install with `--config Debug` or `--config Release` and build the consumer with the same configuration. For Linux, use the matching GCC build; CMake supplies the installed engine library's build-time runtime path to the consumer.

A game needs only:

```cmake
find_package(SONARENGINE CONFIG REQUIRED)
target_link_libraries(MyGame PRIVATE SONARENGINE::SONARENGINE)
```

The SDK contains `include/`, `lib/`, `bin/`, the CMake package, a consumer example, and dependency license notices. `cmake --build out/mingw-debug --target package` creates a ZIP. Keep separate installation prefixes for compiler, architecture, and configuration. On Windows, distribute the engine DLL beside the executable. MinGW SDKs also contain the compiler runtime DLLs used by this build. MSVC consumers require the matching Microsoft runtime; Debug builds require development runtimes.

SONAR's own OpenMP calculations are a separate game dependency. Engine consumers do not inherit OpenMP, raylib, logging, JSON, or platform link requirements through the SDK target.

## API and lifetime contract

The public engine API is C++20. Engine and game must use compatible compiler versions, the same standard library and ABI, architecture, runtime, and Debug/Release configuration. MSVC uses the dynamic CRT (`/MD` or `/MDd`). Rebuild consumers when changing the engine SDK; no cross-version binary compatibility is promised.

Public headers contain only engine and standard-library types. `Sonar` owns rendering/input value types and opaque shared resource handles. Backend conversion, OpenGL state, JSON parsing, and logging implementation stay in the engine. `SONARENGINE_API` marks the shared-library interface.

Texture, shader, render-target, and music handles share backend state. Explicit unload invalidates copies; the final owning handle also releases its resource. Release graphics resources on the render thread before closing the window, and music before closing audio. A render target's texture is a borrowed attachment and must not outlive or be unloaded independently of its target. Image pixels are owned CPU memory. Use `Display::BeginDisplayDraw` / `EndDisplayDraw` in game display subclasses.

`SharedTexture2D` retains the existing resource-manager reference-counting contract and converts only to an engine texture handle. Its resource metadata owner must outlive it. Game objects and callbacks must be destroyed before unloading the engine library.

Class metadata and the reflection registry live in SONARENGINE. Game reflection macros request metadata through that registry, keeping identity consistent across modules. Class names must remain unique, and registration occurs during startup on the game thread.

SONAR owns continent geometry and naval units; the engine provides generic world and line-buffer facilities. Backend rendering calls are unavailable to the game.

## Checks

With `BUILD_TESTING=ON` (the default), every public header is compiled independently. CTest exercises shared-library class identity, game-defined subclasses, events, destruction, logging, texture uploads/readback, and render targets. Run rendering tests under Xvfb on headless Linux. CI configures with `BUILD_TESTING=OFF` and builds and installs the SDK and builds its consumer example without running tests.

Interactive checks still cover menu/options transitions, map zoom/pan and selection, display dragging/resizing, and waterfall continuity. CI checks compilation, SDK installation, and consumer linking on Linux and Windows; runtime checks are performed locally.
