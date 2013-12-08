add_subdirectory(Signals)

set(
	SOURCE 
	${SOURCE}
	${CMAKE_CURRENT_SOURCE_DIR}/CachedResource.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/DebugLogManager.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/DebugMessageHandler.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/RasterizingRenderManager.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/RaytracingRenderManager.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/RenderManager.cpp 
	PARENT_SCOPE 
)