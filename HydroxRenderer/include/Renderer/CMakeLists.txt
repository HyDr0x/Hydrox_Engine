add_subdirectory(Buffer)
add_subdirectory(Pipeline)
add_subdirectory(Resources)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)