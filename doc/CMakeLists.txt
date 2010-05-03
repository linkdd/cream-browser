include(FindDoxygen)
if(DOXYGEN_EXECUTABLE)
    add_custom_target(doc
	COMMAND ${DOXYGEN_EXECUTABLE} ${SOURCE_DIR}/doc/Doxyfile
	WORKING_DIRECTORY ${BUILD_DIR})
endif()
