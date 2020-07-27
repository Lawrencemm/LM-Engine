project(editor)

add_library(
  lmeditor

  src/project_plugin.cpp

  src/app/app.cpp
  src/app/layout.cpp
  src/app/save_load.cpp

  src/app/states/gui.cpp
  src/app/states/modal.cpp

  src/model/selection.cpp

  src/model/trimesh/trimesh.cpp

  src/model/orbital_camera.cpp

  src/controller/viewport/viewport.cpp

  src/controller/visual_registry_editor/visual_registry_controller.cpp
  src/controller/visual_registry_editor/move_selection.cpp
  src/controller/visual_registry_editor/viewport_commands.cpp
  src/controller/visual_registry_editor/move_selection_commands.cpp

  src/controller/visual_registry_editor/states/add_adjacent.cpp
  src/controller/visual_registry_editor/states/translate.cpp
  src/controller/visual_registry_editor/states/select.cpp
  src/controller/visual_registry_editor/states/copy_object.cpp
  src/controller/visual_registry_editor/states/scale.cpp
  src/controller/visual_registry_editor/states/rotate.cpp
  src/controller/visual_registry_editor/states/reparent.cpp

  src/controller/entity_list/entity_list_controller.cpp

  src/controller/inspector/inspector_controller.cpp

  src/component/visuial_registry_editor/rendering.cpp
  src/component/visuial_registry_editor/visual_registry_editor.cpp

  src/component/inspector/inspector_component.cpp

  src/component/axes3d/axes3d.cpp

  src/component/command_help/command_help.cpp

  src/component/saver_modal/saver.cpp

  src/component/entity_list/entity_list_component.cpp

  src/component/asset_list/asset_list_component.cpp

  src/component/player.cpp
)

target_link_libraries(
  lmeditor

  PUBLIC

  lmng
  lmgl
  lmlib
  lmpl
  lmtk
  lmhuv
  fmt::fmt
  OpenMesh::OpenMesh
  yaml-cpp::yaml-cpp
  clara::clara
  Boost::Boost
)

if (UNIX)
  TARGET_LINK_LIBRARIES(lmeditor PUBLIC dl)
endif ()

target_include_directories(
  lmeditor

  PUBLIC
  include

  PRIVATE
  include/lmeditor
  ${embed-resource_INCLUDES}
)

target_compile_definitions(
  lmeditor

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

# In-built shaders
file(GLOB_RECURSE GLSL_SOURCE_FILES "src/shaders/*")

foreach (FILE ${GLSL_SOURCE_FILES})
  file(RELATIVE_PATH FILE_NAME "${PROJECT_SOURCE_DIR}/src/shaders" "${FILE}")
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  get_filename_component(SPIRV_DIR "${SPIRV}" DIRECTORY)
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SPIRV_DIR}"
    COMMAND glslangValidator -V ${FILE} -o ${SPIRV}
    DEPENDS ${FILE}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(
  editor_shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(lmeditor editor_shaders)

embed_built_resources(SHADER_RESOURCES ${SPIRV_BINARY_FILES})

target_sources(lmeditor PRIVATE ${SHADER_RESOURCES})

add_executable(
  Editor
  src/main.cpp
)

target_link_libraries(Editor lmeditor)

# Testing.
add_executable(
  test_lmeditor

  test/test_lmeditor.cpp
  test/test_trimesh.cpp
  test/visual_registry_editor/test.cpp
  test/visual_registry_editor/test_move_selection.cpp
  test/test_entity_list.cpp
)

target_include_directories(
  test_lmeditor

  PRIVATE
  include/lmeditor
)

target_link_libraries(
  test_lmeditor

  lmeditor
  ${OpenMesn_LIBRARIES}
  Catch2::Catch2
)

add_test(NAME lmeditor COMMAND test_lmeditor)
