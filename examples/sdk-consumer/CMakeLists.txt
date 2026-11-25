cmake_minimum_required(VERSION 3.25)
project(EngineConsumer LANGUAGES CXX)
find_package(SONARENGINE CONFIG REQUIRED)
add_executable(engine_consumer main.cpp)
target_link_libraries(engine_consumer PRIVATE SONARENGINE::SONARENGINE)

# Copy all runtime DLLs required by the executable into its output directory.
# This is needed on Windows so engine_consumer.exe can find SONARENGINE.dll
# when it is launched from build/<configuration>.
add_custom_command(TARGET engine_consumer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_RUNTIME_DLLS:engine_consumer>
          $<TARGET_FILE_DIR:engine_consumer>
  COMMAND_EXPAND_LISTS
)

enable_testing()
add_test(NAME engine_consumer COMMAND engine_consumer)
