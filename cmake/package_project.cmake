macro(LM_PACKAGE TARGET INSTALL_BASE)
  install(TARGETS ${TARGET} RUNTIME DESTINATION ${INSTALL_BASE}/bin)
  install(DIRECTORY assets/ DESTINATION ${INSTALL_BASE}/assets)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BASE}/bin)
  include(InstallRequiredSystemLibraries)
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
