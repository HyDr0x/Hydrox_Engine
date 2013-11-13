add_subdirectory(Graphics)
add_subdirectory(Loader)
add_subdirectory(Saver)
add_subdirectory(Services)
add_subdirectory(Utility)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)