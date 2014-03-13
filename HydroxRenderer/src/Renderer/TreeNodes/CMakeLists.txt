add_subdirectory(RenderNodeTemplates)

set(
	SOURCE 
	${SOURCE} 
	${CMAKE_CURRENT_SOURCE_DIR}/GroupNode.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/RenderNode.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/ShaderNode.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/TextureNode.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/TreeNode.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/VertexDeclarationNode.cpp 
	PARENT_SCOPE 
)