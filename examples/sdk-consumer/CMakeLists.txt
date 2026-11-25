cmake_minimum_required(VERSION 3.25)
project(EngineConsumer LANGUAGES CXX)

set(SONARENGINE_SDK_HINTS
  "${CMAKE_CURRENT_LIST_DIR}/../../lib/cmake/SONARENGINE"      # Installed example
  "${CMAKE_CURRENT_LIST_DIR}/../../dist/msvc-release"        # Source-tree example
)

find_package(SONARENGINE CONFIG REQUIRED
  HINTS ${SONARENGINE_SDK_HINTS}
)

add_executable(engine_consumer main.cpp)
target_link_libraries(engine_consumer PRIVATE SONARENGINE::SONARENGINE)
set_target_properties(engine_consumer PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:engine_consumer>"
)

# Copy the example's own minimal resource bundle beside the executable.
add_custom_command(TARGET engine_consumer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_LIST_DIR}/resources"
          "$<TARGET_FILE_DIR:engine_consumer>/resources"
)

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
