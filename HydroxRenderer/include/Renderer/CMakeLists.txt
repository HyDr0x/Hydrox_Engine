add_subdirectory(Buffer)
add_subdirectory(OpenGLDebugLogger)
add_subdirectory(Pipeline)
add_subdirectory(Renderdata)
add_subdirectory(Renderpass)
add_subdirectory(String)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)