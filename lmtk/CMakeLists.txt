project(lmtk)

if (UNIX)
  set(
    PLATFORM_SOURCES
    src/font/linux/font_loader.cpp
  )
  set(
    PLATFORM_LIBRARIES
    fontconfig
  )
elseif (WIN32)
  set(
    PLATFORM_SOURCES
    src/font/windows.cpp
  )
  set(
    PLATFORM_LIBRARIES
    dwrite
  )
endif ()

add_library(
  lmtk
  SHARED

  ${PLATFORM_SOURCES}
  src/text_layout.cpp
  src/event.cpp
  src/shapes.cpp
  src/text_editor.cpp
  src/rect_border.cpp
  src/app.cpp
  src/choice_list.cpp
  src/char_field.cpp
  src/font/font.cpp
  src/resource_cache.cpp
)

target_include_directories(
  lmtk
  PUBLIC
  include
)

target_link_libraries(
  lmtk
  lmgl
  lmpl
  lmlib
  embed-resource::embed-resource
  Freetype::Freetype
  ${PLATFORM_LIBRARIES}
  scope_guard::scope_guard
  Boost::Boost
  EnTT::EnTT
  SDL2::SDL2
)

target_compile_definitions(
  lmtk

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

# In-built shaders
file(GLOB_RECURSE GLSL_SOURCE_FILES "src/shaders/*")

foreach (FILE ${GLSL_SOURCE_FILES})
  file(RELATIVE_PATH FILE_NAME "${PROJECT_SOURCE_DIR}/src/shaders" "${FILE}")
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND glslangValidator -V ${FILE} -o ${SPIRV}
    DEPENDS ${FILE}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(
  shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)

embed_built_resources(SHADER_RESOURCES ${SPIRV_BINARY_FILES})

target_sources(
  lmtk PRIVATE ${SHADER_RESOURCES}
)
