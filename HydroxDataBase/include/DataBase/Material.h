#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <map>
#include <vector>
#include <assert.h>

#include "DataBase/DLLExport.h"

#include "DataBase/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace db
  {
    class GRAPHICAPI Material : public ManagedResource
    {
    public:

      enum TextureType
      {
        DIFFUSETEX,
        NORMALTEX,
        DISPLACEMENTTEX,
        SPECULARTEX,
        TEXTURETYPENUM
      };

      static const unsigned int TEXTURENUMBER = 4;

      struct MaterialData
      {
        MaterialData() : diffuseStrength(1.0f), specularStrength(1.0f), ambientStrength(1.0f), specularExponent(1.0f)
        {
        }

        MaterialData(float in_diffuseStrength, float in_specularStrength, float in_ambientStrength, float in_specularExponent) :
          diffuseStrength(in_diffuseStrength),
          specularStrength(in_specularStrength),
          ambientStrength(in_ambientStrength),
          specularExponent(in_specularExponent)
        {
        }

        float diffuseStrength;
        float specularStrength;//metall materials have their own color as specular reflection
        float ambientStrength;
        float specularExponent;
      };

      Material();
      Material(const Material& other);
      Material(MaterialData& materialData, const std::vector< std::vector<util::ResourceHandle> >& textureIndices, util::ResourceHandle shader, bool transparency);
      ~Material();

      Material& operator=(Material other);

      void free();

      void setTextureNumber(TextureType texType, unsigned int texNum);
      unsigned int getTextureNumber(TextureType texType) const;

      void setTextureHandle(TextureType texType, unsigned int slot, util::ResourceHandle textureHandle);
      util::ResourceHandle getTextureHandle(TextureType texType, unsigned int slot) const;

      void setShaderHandle(util::ResourceHandle shaderHandle);
      util::ResourceHandle getShaderHandle() const;

      void setMaterialData(const MaterialData& material);
      const MaterialData& getMaterialData() const;

      bool getTransparency() const;

    private:

      void swap(Material& other);

      std::vector< std::vector<util::ResourceHandle> > m_textureHandles;
      util::ResourceHandle m_shaderHandle;//specifies the Shader of the submesh in the Shadermanager for the renderpass

      MaterialData m_materialData;

      bool m_transparency;
    };
  }
}

#endif
