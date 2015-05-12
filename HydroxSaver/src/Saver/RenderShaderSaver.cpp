#include "Saver/RenderShaderSaver.h"

#include <fstream>
#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace saver
  {
    void RenderShaderSaver::save(std::string path, std::string filename, sh::RenderShaderHandle renderShaderHandle, util::SingletonManager *singletonManager)
    {
      const sh::RenderShader& shader = singletonManager->getService<sh::ShaderContainer>()->getRenderShader(renderShaderHandle);

      std::vector<std::string> shaderType(5);
      shaderType[0] = ".vert";
      shaderType[1] = ".frag";
      shaderType[3] = ".geom";
      shaderType[4] = ".tctrl"; 
      shaderType[2] = ".teval";

      std::vector<std::string> shaderSources = shader.getShaderSources();

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