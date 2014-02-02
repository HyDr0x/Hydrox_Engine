add_subdirectory(Scene)
add_subdirectory(Traverser)
add_subdirectory(TreeNodes)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)