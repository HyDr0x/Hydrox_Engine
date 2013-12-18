#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <map>
#include <vector>
#include <assert.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class GRAPHICAPI Material : public CachedResource
  {
  public:

    enum TextureType
	  {
		  DIFFUSETEX,
		  NORMALTEX,
		  DISPLACEMENTTEX,
		  SPECULARTEX
	  };

    struct MaterialData
    {
      MaterialData() : diffuseStrength(1.0f), specularStrength(1.0f), ambientStrength(1.0f), specularExponent(1.0f), transparency(false), color(Vector<float, 3>::identity())
      {
      }

      MaterialData(float in_diffuseStrength, float in_specularStrength, float in_ambientStrength, float in_specularExponent, bool in_transparency, Vector<float, 3> in_color = Vector<float, 3>::identity()) :
        diffuseStrength(in_diffuseStrength),
        specularStrength(in_specularStrength),
        ambientStrength(in_ambientStrength),
        specularExponent(in_specularExponent),
        transparency(in_transparency),
        color(in_color)
      {
      }

      Vector<float, 3> color;
      float diffuseStrength;
      float specularStrength;//metall materials have their own color as specular reflection
      float ambientStrength;
      float specularExponent;
      bool transparency;//transparency y/n
    };

    Material();
    Material(const Material& o);
    Material(MaterialData& materialData, std::vector< std::vector<ResourceHandle> >& textureIndices, ResourceHandle shader);
    Material& operator=(const Material& o);
	  ~Material();

    void free();

	  void setTextureNumber(TextureType texType, unsigned int texNum);
	  unsigned int getTextureNumber(TextureType texType) const;

	  void setTexture(TextureType texType, unsigned int slot, ResourceHandle textureIndex);
	  ResourceHandle getTexture(TextureType texType, unsigned int slot) const;

    void setShader(ResourceHandle shaderIndex);
    ResourceHandle getShader() const;

    void setMaterial(MaterialData& material);
    const MaterialData& getMaterial() const;

  private:

    std::vector< std::vector<ResourceHandle> > m_textureIndices;
    ResourceHandle m_shaderIndex;//specifies the Shader of the submesh in the Shadermanager for the renderpass

    MaterialData m_materialData;
  };
}

#endif