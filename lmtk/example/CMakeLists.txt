project(lmtk_demo)

file(GLOB DEMO_SOURCE_FILES "*.cpp")

foreach (SOURCE_FILE ${DEMO_SOURCE_FILES})
  get_filename_component(DEMO_NAME ${SOURCE_FILE} NAME_WE)
  set(DEMO_NAME ${DEMO_NAME}_example)
  add_executable(${DEMO_NAME} ${SOURCE_FILE})
  target_link_libraries(${DEMO_NAME} lmtk)
endforeach ()
