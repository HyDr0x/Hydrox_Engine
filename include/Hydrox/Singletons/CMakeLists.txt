add_subdirectory(Signals)

set(
	HEADER 
	${HEADER}
	${CMAKE_CURRENT_SOURCE_DIR}/CachedResource.h 
	${CMAKE_CURRENT_SOURCE_DIR}/CacheManager.hpp 
	${CMAKE_CURRENT_SOURCE_DIR}/DebugLogManager.h 
	${CMAKE_CURRENT_SOURCE_DIR}/DebugMessageHandler.h 
	${CMAKE_CURRENT_SOURCE_DIR}/RasterizerRenderManager.h 
	${CMAKE_CURRENT_SOURCE_DIR}/RaytracingRenderManager.h 
	${CMAKE_CURRENT_SOURCE_DIR}/RenderManager.h 
	${CMAKE_CURRENT_SOURCE_DIR}/Singleton.hpp 
	PARENT_SCOPE 
)