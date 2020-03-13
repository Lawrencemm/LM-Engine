project(editor)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MODULE_PATH})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_STANDARD 17)
else ()
  set(CMAKE_CXX_STANDARD 20)
endif ()

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
  src/app/states/gui.cpp
  src/app/states/player.cpp
  src/app/states/modal.cpp
  src/app/inspector_handler.cpp
  src/app/map_editor_handler.cpp

  src/viewport/orbital_camera.cpp
  src/trimesh/trimesh.cpp

  src/map_editor/map_editor.cpp
  src/map_editor/model/model.cpp
  src/map_editor/model/move_selection.cpp
  src/map_editor/model/save_load.cpp
  src/map_editor/model/states/add_adjacent.cpp
  src/map_editor/model/states/translate.cpp
  src/map_editor/model/states/select.cpp
  src/map_editor/model/states/copy_object.cpp
  src/map_editor/model/viewport_commands.cpp
  src/map_editor/model/states/scale.cpp
  src/map_editor/model/states/rotate.cpp
  src/map_editor/view/rendering.cpp
  src/map_editor/view/view.cpp
  src/map_editor/model/move_selection_commands.cpp
  src/map_editor/model/states/reparent.cpp

  src/axes3d/axes3d.cpp

  src/viewport/viewport.cpp

  src/inspector/inspector.cpp
  src/inspector/add_state.cpp
  src/command_help/command_help.cpp

  src/map_selector.cpp
  src/app/map_saver.cpp

  src/entity_list/entity_list.cpp
  src/entity_list/view.cpp
  src/app/entity_list_handle.cpp)

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

  src/test_lmeditor.cpp
  src/trimesh/test_trimesh.cpp
  src/map_editor/model/test/test.cpp
  src/map_editor/model/test/test_move_selection.cpp
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
