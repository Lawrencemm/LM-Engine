macro(LM_PACKAGE TARGET INSTALL_BASE)
  list(APPEND pre_exclude_regexes api-ms-*)
  list(APPEND pre_exclude_regexes ext-ms-*)
  list(APPEND pre_exclude_regexes hvsifiletrust.dll)
  install(
    TARGETS ${TARGET}
    RUNTIME_DEPENDENCIES PRE_EXCLUDE_REGEXES ${pre_exclude_regexes}
    RUNTIME DESTINATION ${INSTALL_BASE}/bin
    LIBRARY DESTINATION ${INSTALL_BASE}/bin
  )
  install(DIRECTORY assets/ DESTINATION ${INSTALL_BASE}/assets)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BASE}/bin)
  include(InstallRequiredSystemLibraries)
  install(CODE
      "
      file(GLOB SOS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*.so*)
      file(COPY \${SOS}\ DESTINATION ${INSTALL_BASE}/bin)
    "
  )
  install(CODE
      "
      file(GLOB DLLS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*.dll)
      file(COPY \${DLLS}\ DESTINATION ${INSTALL_BASE}/bin)
    "
  )
  install(CODE
      "
    file(GLOB PDBS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*.pdb)
    file(COPY \${PDBS}\ DESTINATION ${INSTALL_BASE}/bin)
      "
  )
endmacro()
