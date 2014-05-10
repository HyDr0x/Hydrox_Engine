project(Hydrox_Renderer)

set(
	LIBSUFFIX 
	${CMAKE_STATIC_LIBRARY_SUFFIX}
)

add_definitions(-DDLL_EXPORT)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(include/Renderer)
add_subdirectory(src/Renderer)

add_library(Hydrox_Renderer SHARED ${SOURCE} ${HEADER})

find_package(GLEW)
find_package(OPENGL)

set(
	INCLUDES 
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${HYDROX_UTIL_INCLUDE} 
	${HYDROX_XBAR_INCLUDE} 
	${GLEW_INCLUDE_DIRS} 
)

include_directories(${INCLUDES})

set(
	DEBUG_LIBRARIES 
	debug ${HYDROX_UTIL_LIB_DBG}${LIBSUFFIX} 
	debug ${HYDROX_XBAR_LIB_DBG}${LIBSUFFIX} 
	debug ${GLEW_LIBRARIES}
)

set(
	RELEASE_LIBRARIES 
	optimized ${HYDROX_UTIL_LIB_REL}${LIBSUFFIX} 
	optimized ${HYDROX_XBAR_LIB_REL}${LIBSUFFIX} 
	optimized ${GLEW_LIBRARIES} 
)

set(
	GENERAL_LIBRARIES 
	general ${OPENGL_gl_LIBRARY}${LIBSUFFIX} 
)

target_link_libraries(Hydrox_Renderer ${DEBUG_LIBRARIES})
target_link_libraries(Hydrox_Renderer ${RELEASE_LIBRARIES})
target_link_libraries(Hydrox_Renderer ${GENERAL_LIBRARIES})

#source_group("Header Files//Graphics" FILES ${HEADER})

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
                STRING( REPLACE "/" "////" group_name ${ARGV0} )
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