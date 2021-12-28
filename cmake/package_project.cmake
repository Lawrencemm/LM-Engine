macro(LM_PACKAGE TARGET)
  install(
    TARGETS ${TARGET}
    COMPONENT ${TARGET}_package
    EXCLUDE_FROM_ALL
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
  )
  install(DIRECTORY assets/ DESTINATION assets COMPONENT ${TARGET}_package EXCLUDE_FROM_ALL)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT ${TARGET}_package)
  include(InstallRequiredSystemLibraries)
  if (WIN32)
    install(CODE
      "
        file(
          GET_RUNTIME_DEPENDENCIES
          RESOLVED_DEPENDENCIES_VAR _CMAKE_DEPS
          EXECUTABLES \$<TARGET_FILE:${TARGET}>
          PRE_EXCLUDE_REGEXES api-ms-* ext-ms-* hvsifiletrust.dll
          POST_EXCLUDE_REGEXES \".*system32/.*\\\\.dll\"
          DIRECTORIES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )
        file(COPY \${_CMAKE_DEPS}\ DESTINATION \${CMAKE_INSTALL_PREFIX}/bin)
      "
      COMPONENT ${TARGET}_package
      EXCLUDE_FROM_ALL
    )
  else()
    install(CODE "
      file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES $<TARGET_FILE:${TARGET}>
        RESOLVED_DEPENDENCIES_VAR _r_deps
        UNRESOLVED_DEPENDENCIES_VAR _u_deps
      )
      foreach(_file \${_r_deps})
        file(INSTALL
          DESTINATION \"\${CMAKE_INSTALL_PREFIX}/bin\"
          TYPE SHARED_LIBRARY
          FOLLOW_SYMLINK_CHAIN
          FILES \"\${_file}\"
        )
      endforeach()
      list(LENGTH _u_deps _u_length)
      if(\"\${_u_length}\" GREATER 0)
        message(WARNING \"Unresolved dependencies detected!\")
      endif()
    "
    COMPONENT ${TARGET}_package
    EXCLUDE_FROM_ALL
  )
  endif()
endmacro()
