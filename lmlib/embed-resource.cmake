# Call this with the output of custom commands that buld resources
# you want to embed. The paths should be absolute.
function(embed_built_resources out_var)
    set(result)
    foreach (in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_CURRENT_BINARY_DIR} "${in_f}")
        set(out_f ${in_f}.cpp)
        get_filename_component(out_f_dir "${out_f}" DIRECTORY)
        add_custom_command(
                OUTPUT ${out_f}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${out_f_dir}"
                COMMAND embed-resource "${out_f}" "${src_f}"
                DEPENDS "${in_f}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Building binary file for embedding ${out_f}"
                VERBATIM)
        list(APPEND result "${out_f}")
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

# Call this with relative paths to sources you want to embed as resources.
function(embed_resources out_var)
    set(result)
    foreach (in_f ${ARGN})
        file(RELATIVE_PATH src_f embed-resource "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.cpp")
        get_filename_component(out_f_dir "${out_f}" DIRECTORY)
        add_custom_command(
                OUTPUT ${out_f}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${out_f_dir}"
                COMMAND embed-resource "${out_f}" "${src_f}"
                DEPENDS "${in_f}"
                WORKING_DIRECTORY embed-resource
                COMMENT "Building binary file for embedding ${out_f}"
                VERBATIM)
        list(APPEND result "${out_f}")
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()