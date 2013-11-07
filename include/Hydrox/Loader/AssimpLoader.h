#ifndef ASSIMPLOADER_H_
#define ASSIMPLOADER_H_

#include <stack>
#include <string>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Mesh.h"

#include "Hydrox/Services/CacheManager.hpp"

class ILDevilLoader;

class GroupNode;
class TreeNode;

class Camera;

class Scene;

class GRAPHICAPI AssimpLoader
{
public:

  AssimpLoader(Camera *camera, ModelManager *modelManager, MaterialManager *materialManager, TextureManager *textureManager, ShaderManager *shaderManager);
  AssimpLoader(const AssimpLoader& o);
  AssimpLoader& operator=(const AssimpLoader& o);
  ~AssimpLoader();

  Scene* load(std::string filename, std::string materialFileName, bool yAxisFlipped = true);

private:

  AssimpLoader(){}

  void loadAnimations();
  ResourceHandle loadVertices(const aiMesh *mesh, ResourceHandle materialIndex, bool yAxisFlipped);
  void loadMeshesFromAssimp(std::vector<ResourceHandle>& meshes, std::string materialFileName, const aiScene *scene, bool yAxisFlipped);
  ResourceHandle loadMaterialsFromAssimp(std::string materialFileName, const aiScene *model, unsigned int id);
  GroupNode* loadSceneGraphFromAssimp(std::string filename, const aiNode *rootNode, std::vector<ResourceHandle> meshes);

  TreeNode* createSceneNodes(const aiNode *node, std::vector<ResourceHandle> meshes, GroupNode *parentNode, TreeNode *nextSibling);

	std::string m_modelPath;

  Camera *m_camera;
  ModelManager *m_modelManager;
  MaterialManager *m_materialManager;
  TextureManager *m_textureManager;
  ShaderManager *m_shaderManager;
};

#endif