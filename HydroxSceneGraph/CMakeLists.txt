project(Hydrox_SceneGraph)

set(
	LIBSUFFIX 
	${CMAKE_STATIC_LIBRARY_SUFFIX}
)

add_definitions(-DDLL_EXPORT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(include/SceneGraph)
add_subdirectory(src/SceneGraph)

add_library(Hydrox_SceneGraph SHARED ${SOURCE} ${HEADER})

find_package(GLEW)
find_package(OPENGL)

set(HYDROX_UTIL_INCLUDE "default value" CACHE FILEPATH "Path to the util include directory.")
set(HYDROX_UTIL_LIB_DBG "default value" CACHE FILEPATH "Path to the util debug library directory.")
set(HYDROX_UTIL_LIB_REL "default value" CACHE FILEPATH "Path to the util release library directory.")

set(
	INCLUDES 
	${HYDROX_UTIL_INCLUDE} 
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${GLEW_INCLUDE_DIRS} 
)

include_directories(${INCLUDES})

set(
	DEBUG_LIBRARIES 
	debug ${HYDROX_UTIL_LIB_DBG} 
	debug ${GLEW_LIBRARY}/glew32d${LIBSUFFIX} 
)

set(
	RELEASE_LIBRARIES 
	optimized ${HYDROX_UTIL_LIB_REL} 
	optimized ${GLEW_LIBRARY}/glew32${LIBSUFFIX} 
)

set(
	GENERAL_LIBRARIES 
	general ${OPENGL_gl_LIBRARY}${LIBSUFFIX} 
)

target_link_libraries(Hydrox_SceneGraph ${DEBUG_LIBRARIES})
target_link_libraries(Hydrox_SceneGraph ${RELEASE_LIBRARIES})
target_link_libraries(Hydrox_SceneGraph ${GENERAL_LIBRARIES})

#source_group("Header Files\\Graphics" FILES ${HEADER})

MACRO( source_group_tree )
        FILE( GLOB all_files ${ARGV1}/* )
        SET( files "" )
        FOREACH( each ${all_files} )
                IF( NOT IS_DIRECTORY ${each} )
                        GET_FILENAME_COMPONENT( file_name ${each} NAME )
                        LIST( APPEND files "${ARGV1}/${file_name}" )
                ENDIF()
        ENDFOREACH()
        LIST( LENGTH files files_len )
        IF( ${files_len} GREATER 0 )
                STRING( REPLACE "/" "\\\\" group_name ${ARGV0} )
                SOURCE_GROUP("${group_name}" FILES ${files})
        ENDIF()
                
        FILE( GLOB directories ${ARGV1}/*/ )
        FOREACH( each ${directories} )
                IF( IS_DIRECTORY ${each} )
                        GET_FILENAME_COMPONENT( file_name ${each} NAME )
                        source_group_tree("${ARGV0}/${file_name}" "${ARGV1}/${file_name}")
                ENDIF()
        ENDFOREACH()
ENDMACRO()

source_group_tree("Header" include)
source_group_tree("Source" src)