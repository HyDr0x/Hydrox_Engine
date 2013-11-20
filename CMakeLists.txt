cmake_minimum_required (VERSION 2.6)

project(Hydrox_Engine)

set(
	LIBSUFFIX 
	${CMAKE_STATIC_LIBRARY_SUFFIX}
)

add_definitions(-DDLL_EXPORT)

add_subdirectory(include/Hydrox)

add_subdirectory(src/Hydrox)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(Hydrox_Engine SHARED ${SOURCE} ${HEADER})

find_package(GLEW)
find_package(OPENGL)
find_package(DEVIL)
find_package(ASSIMP)

set(
	INCLUDES 
	${IL_INCLUDE_DIR} 
	${GLEW_INCLUDE_DIRS} 
	${ASSIMP_DIR}/include 
)

include_directories(${INCLUDES})

set(
	DEBUG_LIBRARIES 
	debug ${GLEW_LIBRARY}/glew32d${LIBSUFFIX} 
	debug ${ASSIMP_DIR}/lib/x64/Debug/assimpD${LIBSUFFIX} 
)

set(
	RELEASE_LIBRARIES 
	optimized ${GLEW_LIBRARY}/glew32${LIBSUFFIX} 
	optimized ${ASSIMP_DIR}/lib/x64/Release/assimp${LIBSUFFIX} 
)

set(
	GENERAL_LIBRARIES 
	general ${IL_LIBRARIES}/DevIL${LIBSUFFIX} 
	general ${OPENGL_gl_LIBRARY}${LIBSUFFIX} 
)

target_link_libraries(Hydrox_Engine ${DEBUG_LIBRARIES})
target_link_libraries(Hydrox_Engine ${RELEASE_LIBRARIES})
target_link_libraries(Hydrox_Engine ${GENERAL_LIBRARIES})

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