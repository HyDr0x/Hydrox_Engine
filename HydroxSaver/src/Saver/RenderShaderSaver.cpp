#include "Saver/RenderShaderSaver.h"

#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/ResourceManager.hpp>
#include <Renderer/Resources/RenderShader.h>

namespace he
{
	namespace saver
	{
    void RenderShaderSaver::save(std::string path, std::string filename, util::ResourceHandle renderShaderHandle, util::SingletonManager *singletonManager)
    {
      renderer::RenderShader *shader = singletonManager->getService<renderer::RenderShaderManager>()->getObject(renderShaderHandle);

      std::vector<std::string> shaderSources = shader->getShaderSources();
      for(unsigned int i = 0; i < shaderSources.size(); i++)
      {
        if(!shaderSources[i].empty())
        {
          std::ofstream fileStream;
          fileStream.open(path + filename, std::ofstream::out |std::ofstream::binary);
          fileStream << shaderSources[i];
          fileStream.close();
        }
      }
    }
  }
}