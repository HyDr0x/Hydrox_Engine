#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include <vector>
#include <list>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/Io_service.h"
#include "Hydrox/Services/CachedResource.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Shader.h"
#include "Hydrox/Graphics/Texture.h"
#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Graphics/Sprite.h"

template<class CLASS> class GRAPHICAPI CacheManager : public Io_service
{
public:

	CacheManager(std::string path)
  {
	  m_path = path;
    m_availableBorder = 0;
  }

	~CacheManager()
  {
    for(unsigned int i = 0; i < m_objectsCache.size(); i++)
    {
      m_objectsCache[i].free();
    }

	  m_objectsCache.clear();
    m_list.clear();
  }

  CLASS* getObject(ResourceHandle objectID)
  {
    return dynamic_cast<CLASS*>(&m_objectsCache[objectID]);
  }

  ResourceHandle addObject(CLASS& object)
  {
    if(m_objectsCache.size() <= m_availableBorder)
    {
      unsigned int size = static_cast<unsigned int>((m_objectsCache.size() / m_BLOCKSIZE + 1) * m_BLOCKSIZE);
      m_objectsCache.resize(size);
    }

    ResourceHandle id;

    if(m_list.empty())
    {
      id = m_availableBorder;
      m_availableBorder++;
    }
    else
    {
      id = *m_list.begin();
      m_list.pop_front();
    }

    m_objectsCache[id] = object;

    return id;
  }

	void deleteObject(ResourceHandle objectID)
  {
    if(objectID != m_availableBorder - 1)
    {
      m_list.push_front(objectID);
    }
    else
    {
      m_availableBorder--;
    }
  }

	std::string getPath() const
  {
	  return m_path;
  }

private:

	CacheManager(){}
	CacheManager(const CacheManager&){}

  static const unsigned int m_BLOCKSIZE = 64;

  unsigned int m_availableBorder;
	std::list<ResourceHandle> m_list;

	std::vector<CLASS> m_objectsCache;

  std::string m_path;
};

template class GRAPHICAPI CacheManager<Mesh>;
template class GRAPHICAPI CacheManager<Material>;
template class GRAPHICAPI CacheManager<Shader>;
template class GRAPHICAPI CacheManager<Texture>;
template class GRAPHICAPI CacheManager<Billboard>;
template class GRAPHICAPI CacheManager<Sprite>;

typedef CacheManager<Mesh> ModelManager;
typedef CacheManager<Material> MaterialManager;
typedef CacheManager<Shader> ShaderManager;
typedef CacheManager<Texture> TextureManager;
typedef CacheManager<Billboard> BillboardManager;
typedef CacheManager<Sprite> SpriteManager;

#endif