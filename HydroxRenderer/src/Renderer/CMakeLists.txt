add_subdirectory(Buffer)
add_subdirectory(OpenGLDebugLog)
add_subdirectory(Pipeline)
add_subdirectory(String)
add_subdirectory(TreeNodes)
add_subdirectory(Traverser)

set(
	SOURCE 
	${SOURCE} 
	${CMAKE_CURRENT_SOURCE_DIR}/GPUCode.cpp 
	PARENT_SCOPE 
)