project(lmtk)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(lmgl REQUIRED)
find_package(lmlib REQUIRED)
find_package(lmpl REQUIRED)
find_package(embed-resource REQUIRED)
find_package(Freetype REQUIRED)
find_package(scope_guard REQUIRED)
find_package(Boost REQUIRED)
find_package(readerwriterqueue REQUIRED)
find_package(EnTT REQUIRED)

add_executable(embed-resource IMPORTED)
set_target_properties(embed-resource PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/embed-resource)

add_executable(glslangValidator IMPORTED)
set_target_properties(glslangValidator PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glslangValidator)

include(${CMAKE_CURRENT_BINARY_DIR}/scripts/embed-resource.cmake)

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
  src/input_event.cpp
  src/shapes.cpp
  src/text_editor.cpp
  src/rect_border.cpp
  src/app_flow_graph.cpp
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
  lmgl::lmgl
  lmpl::lmpl
  lmlib::lmlib
  embed-resource::embed-resource
  Freetype::Freetype
  ${PLATFORM_LIBRARIES}
  scope_guard::scope_guard
  Boost::Boost
  readerwriterqueue::readerwriterqueue
  EnTT::EnTT
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

add_subdirectory(example)
