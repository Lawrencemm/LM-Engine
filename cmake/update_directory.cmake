file(GLOB_RECURSE _file_list RELATIVE "${src_dir}" "${src_dir}/*")

foreach( each_file ${_file_list} )
  set(destinationfile "${dst_dir}/${each_file}")
  set(sourcefile "${src_dir}/${each_file}")
  execute_process( COMMAND ${CMAKE_COMMAND} -E compare_files ${destinationfile} ${sourcefile}
    RESULT_VARIABLE compare_result
  )

  if(NOT EXISTS ${destinationfile} OR compare_result EQUAL 1)
    get_filename_component(destinationdir ${destinationfile} DIRECTORY)
    file(COPY ${sourcefile} DESTINATION ${destinationdir})
  endif()
endforeach(each_file)
