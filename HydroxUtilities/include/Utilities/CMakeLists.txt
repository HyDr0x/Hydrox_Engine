add_subdirectory(Math)
add_subdirectory(Miscellaneous)
add_subdirectory(Observer)
add_subdirectory(Pointer)
add_subdirectory(PrimitiveGenerator)
add_subdirectory(Signals)
add_subdirectory(Timer)
add_subdirectory(Tree)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/DLLExport.h 
	PARENT_SCOPE 
)