#include "Saver/MaterialSaver.h"

#include <sstream>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/Material.h>
#include <Renderer/Resources/Texture2D.h>
#include <Renderer/Resources/RenderShader.h>

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

namespace he
{
	namespace saver
	{    MaterialSaver::MaterialSaver(util::SingletonManager *singletonManager)    {      m_textureManager = singletonManager->getService<renderer::TextureManager>();      m_renderShaderManager = singletonManager->getService<renderer::RenderShaderManager>();    }    void MaterialSaver::saveMaterial(std::string fileName, const renderer::Material* material)
    {

    }
	}
}
