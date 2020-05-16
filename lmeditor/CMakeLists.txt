project(editor)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MODULE_PATH})

find_package(lmengine REQUIRED)
find_package(lmgl REQUIRED)
find_package(lmlib REQUIRED)
find_package(lmpl REQUIRED)
find_package(lmtk REQUIRED)
find_package(lmhuv REQUIRED)
find_package(embed-resource REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenMesh REQUIRED)
add_executable(glslangValidator IMPORTED)
set_target_properties(glslangValidator PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glslangValidator)
add_executable(embed-resource IMPORTED)
set_target_properties(embed-resource PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/embed-resource)
find_package(yaml-cpp REQUIRED)
find_package(clara REQUIRED)
find_package(Boost REQUIRED COMPONENTS dll)

include(${CMAKE_CURRENT_BINARY_DIR}/scripts/embed-resource.cmake)

add_library(
  lmeditor

  src/app/resources.cpp
  src/app/app.cpp
  src/app/save_load_map.cpp

  src/app/states/gui.cpp
  src/app/states/player.cpp
  src/app/states/modal.cpp

  src/model/selection.cpp

  src/model/trimesh/trimesh.cpp

  src/model/orbital_camera.cpp

  src/controller/viewport/viewport.cpp

  src/controller/map_editor/map_editor_controller.cpp
  src/controller/map_editor/move_selection.cpp
  src/controller/map_editor/viewport_commands.cpp
  src/controller/map_editor/move_selection_commands.cpp

  src/controller/map_editor/states/add_adjacent.cpp
  src/controller/map_editor/states/translate.cpp
  src/controller/map_editor/states/select.cpp
  src/controller/map_editor/states/copy_object.cpp
  src/controller/map_editor/states/scale.cpp
  src/controller/map_editor/states/rotate.cpp
  src/controller/map_editor/states/reparent.cpp

  src/controller/entity_list/entity_list_controller.cpp

  src/controller/inspector/inspector_controller.cpp

  src/component/map_editor/rendering.cpp
  src/component/map_editor/map_editor_component.cpp

  src/component/inspector/inspector_component.cpp

  src/component/axes3d/axes3d.cpp

  src/component/command_help/command_help.cpp

  src/component/saver_modal/saver.cpp

  src/component/entity_list/entity_list_component.cpp
)

target_link_libraries(
  lmeditor

  PUBLIC

  lmengine::lmengine
  lmgl::lmgl
  lmlib::lmlib
  lmpl::lmpl
  lmtk::lmtk
  lmhuv::lmhuv
  fmt::fmt
  OpenMesh::OpenMesh
  readerwriterqueue::readerwriterqueue
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
  test/map_editor/test.cpp
  test/map_editor/test_move_selection.cpp
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
