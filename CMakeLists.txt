cmake_minimum_required (VERSION 2.6) 

project(Hydrox_Utilities) 
add_subdirectory(HydroxUtilities) 

project(Hydrox_Renderer) 
add_subdirectory(HydroxRenderer) 
add_dependencies(Hydrox_Renderer Hydrox_Utilities Hydrox_SceneGraph)

project(Hydrox_SceneGraph) 
add_subdirectory(HydroxSceneGraph) 
add_dependencies(Hydrox_SceneGraph Hydrox_Utilities)

project(Hydrox_Loader) 
add_subdirectory(HydroxLoader) 
add_dependencies(Hydrox_Loader Hydrox_Utilities Hydrox_SceneGraph Hydrox_Renderer)

project(Sandbox) 
add_subdirectory(Sandbox) 
add_dependencies(Sandbox Hydrox_Loader Hydrox_Utilities Hydrox_SceneGraph Hydrox_Renderer)