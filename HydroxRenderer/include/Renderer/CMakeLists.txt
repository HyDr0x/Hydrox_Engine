add_subdirectory(OpenGLDebugLogger)
add_subdirectory(Buffer)
add_subdirectory(Pipeline)
add_subdirectory(Resources)
add_subdirectory(String)
add_subdirectory(TreeNodes)
add_subdirectory(Traverser)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)