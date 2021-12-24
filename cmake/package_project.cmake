macro(LM_PACKAGE TARGET)
  set(INSTALL_BASE ${CMAKE_CURRENT_BINARY_DIR}/packaged)
  install(TARGETS ${TARGET} RUNTIME DESTINATION ${INSTALL_BASE}/bin)
  install(DIRECTORY assets/ DESTINATION ${INSTALL_BASE}/assets)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BASE}/bin)
  include(InstallRequiredSystemLibraries)
  install(CODE
      "
      file(GLOB DLLS ${CMAKE_BINARY_DIR}/*.dll)
      file(COPY \${DLLS}\ DESTINATION ${INSTALL_BASE}/bin)
    "
  )
  install(CODE
      "
    file(GLOB PDBS ${CMAKE_BINARY_DIR}/*.pdb)
    file(COPY \${PDBS}\ DESTINATION ${INSTALL_BASE}/bin)
      "
  )
endmacro()
