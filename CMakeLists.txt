cmake_minimum_required (VERSION 2.6) 

set(CMAKE_PROJECT_NAME HydroxEngine)

set(HYDROX_DIR ${CMAKE_SOURCE_DIR})

set(HYDROX_UTIL_INCLUDE ${HYDROX_DIR}/HydroxUtilities/include)
set(HYDROX_UTIL_LIB_DBG ${HYDROX_DIR}/HydroxUtilities/lib/Debug/Hydrox_Utilities)
set(HYDROX_UTIL_LIB_REL ${HYDROX_DIR}/HydroxUtilities/lib/Release/Hydrox_Utilities)

set(HYDROX_XBAR_INCLUDE ${HYDROX_DIR}/HydroxXBar/include)
set(HYDROX_XBAR_LIB_DBG ${HYDROX_DIR}/HydroxXBar/lib/Debug/Hydrox_XBar)
set(HYDROX_XBAR_LIB_REL ${HYDROX_DIR}/HydroxXBar/lib/Release/Hydrox_XBar)

set(HYDROX_SG_INCLUDE ${HYDROX_DIR}/HydroxSceneGraph/include)
set(HYDROX_SG_LIB_DBG ${HYDROX_DIR}/HydroxSceneGraph/lib/Debug/Hydrox_SceneGraph)
set(HYDROX_SG_LIB_REL ${HYDROX_DIR}/HydroxSceneGraph/lib/Release/Hydrox_SceneGraph)

set(HYDROX_RE_INCLUDE ${HYDROX_DIR}/HydroxRenderer/include)
set(HYDROX_RE_LIB_DBG ${HYDROX_DIR}/HydroxRenderer/lib/Debug/Hydrox_Renderer)
set(HYDROX_RE_LIB_REL ${HYDROX_DIR}/HydroxRenderer/lib/Release/Hydrox_Renderer)

set(HYDROX_LOADER_INCLUDE ${HYDROX_DIR}/HydroxLoader/include)
set(HYDROX_LOADER_LIB_DBG ${HYDROX_DIR}/HydroxLoader/lib/Debug/Hydrox_Loader)
set(HYDROX_LOADER_LIB_REL ${HYDROX_DIR}/HydroxLoader/lib/Release/Hydrox_Loader)

add_subdirectory(HydroxUtilities) 

add_subdirectory(HydroxXBar) 
add_dependencies(Hydrox_XBar Hydrox_Utilities)

add_subdirectory(HydroxRenderer) 
add_dependencies(Hydrox_Renderer Hydrox_Utilities Hydrox_XBar)

add_subdirectory(HydroxSceneGraph) 
add_dependencies(Hydrox_SceneGraph Hydrox_Utilities Hydrox_XBar)

add_subdirectory(HydroxLoader) 
add_dependencies(Hydrox_Loader Hydrox_Utilities Hydrox_SceneGraph Hydrox_Renderer)

add_subdirectory(Sandbox) 
add_dependencies(Sandbox Hydrox_Loader Hydrox_Utilities Hydrox_XBar Hydrox_SceneGraph Hydrox_Renderer)
