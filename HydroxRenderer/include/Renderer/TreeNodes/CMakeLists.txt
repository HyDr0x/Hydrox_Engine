add_subdirectory(RenderNodeDecorator)

set(
	HEADER 
	${HEADER} 
	${CMAKE_CURRENT_SOURCE_DIR}/GroupNode.h 
	${CMAKE_CURRENT_SOURCE_DIR}/RenderNode.h 
	${CMAKE_CURRENT_SOURCE_DIR}/ShaderNode.h 
	${CMAKE_CURRENT_SOURCE_DIR}/TextureNode.h 
	${CMAKE_CURRENT_SOURCE_DIR}/TreeNode.h 
	${CMAKE_CURRENT_SOURCE_DIR}/VertexDeclarationNode.h 
	PARENT_SCOPE 
)