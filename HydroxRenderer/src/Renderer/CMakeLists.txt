add_subdirectory(Buffer)
add_subdirectory(OpenGLDebugLog)
add_subdirectory(Pipeline)
add_subdirectory(Renderdata)
add_subdirectory(Renderpass)
add_subdirectory(String)

set(
	SOURCE 
	${SOURCE} 
	${CMAKE_CURRENT_SOURCE_DIR}/GPUCode.cpp 
	PARENT_SCOPE 
)