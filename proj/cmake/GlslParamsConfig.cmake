if( NOT TARGET GlslParams )
	get_filename_component( GLSLPARAMS_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	add_library( GlslParams ${GLSLPARAMS_SOURCE_PATH}/GlslParams.cpp )

	target_include_directories( GlslParams PUBLIC "${GLSLPARAMS_SOURCE_PATH}" )
	target_include_directories( GlslParams SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()

	target_link_libraries( GlslParams PRIVATE cinder )

endif()
