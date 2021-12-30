project(lmhuv)

add_library(
  lmhuv
  SHARED

  src/visual.cpp
  src/rendering/shapes.cpp
  src/trimesh/trimesh.cpp
  src/box.cpp
)
target_include_directories(
  lmhuv
  PUBLIC
  include

  PRIVATE
  ${embed-resource_INCLUDES}
)
target_link_libraries(
  lmhuv
  PUBLIC

  lmng
  lmgl
  OpenMesh::OpenMesh
)

target_compile_definitions(
  lmhuv

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
  lmhuv_shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(lmhuv lmhuv_shaders)

embed_built_resources(SHADER_RESOURCES ${SPIRV_BINARY_FILES})

target_sources(lmhuv PRIVATE ${SHADER_RESOURCES})
