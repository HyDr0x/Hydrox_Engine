#include "Saver/RenderShaderSaver.h"

#include <fstream>
#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/RenderShader.h>

namespace he
{
  namespace saver
  {
    void RenderShaderSaver::save(std::string path, std::string filename, util::ResourceHandle renderShaderHandle, util::SingletonManager *singletonManager)
    {
      db::RenderShader *shader = singletonManager->getService<db::RenderShaderManager>()->getObject(renderShaderHandle);

      std::vector<std::string> shaderType(5);
      shaderType[0] = ".vert";
      shaderType[1] = ".tctrl";
      shaderType[2] = ".teval";
      shaderType[3] = ".geom";
      shaderType[4] = ".frag";

      std::vector<std::string> shaderSources = shader->getShaderSources();

      std::stringstream stream;
      for(unsigned int i = 0; i < shaderSources.size(); i++)
      {
        if(!shaderSources[i].empty())
        {
          std::ofstream fileStream;
          stream << i;
          fileStream.open(path + filename + shaderType[i], std::ofstream::out | std::ofstream::trunc);
          fileStream << shaderSources[i];
          fileStream.close();

          stream.str("");
        }
      }
    }
  }
}