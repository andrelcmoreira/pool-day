cmake_minimum_required(VERSION 3.18)

function(enable_doc)
  find_package(Doxygen)

  if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    message("Building library documentation")

    add_custom_target(doc_doxygen ALL
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating library documentation"
                      VERBATIM)
  else(DOXYGEN_FOUND)
    message("Doxygen need to be installed")
  endif(DOXYGEN_FOUND)
endfunction(enable_doc)
