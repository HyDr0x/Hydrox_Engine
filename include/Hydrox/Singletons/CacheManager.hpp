#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include <vector>
#include <list>

#include "Hydrox/Utility/Observer/Observer.hpp"
#include "Hydrox/Singletons/Singleton.hpp"
#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Shader.h"
#include "Hydrox/Graphics/Texture.h"
#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Graphics/Sprite.h"

namespace he
{
  template<class CLASS> class CacheManager : public Singleton<CacheManager<CLASS>>, public Observer<ResourceHandle*>
  {
  public:

    CacheManager()
    {
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

    void initialize(std::string path)
    {
      m_path = path;
    }

    CLASS* getObject(ResourceHandle handle)
    {
      return dynamic_cast<CLASS*>(&m_objectsCache[handle.getID()]);
    }

    ResourceHandle addObject(CLASS& object)
    {
      if(m_objectsCache.size() <= m_availableBorder)
      {
        unsigned int size = static_cast<unsigned int>((m_objectsCache.size() / m_BLOCKSIZE + 1) * m_BLOCKSIZE);
        m_objectsCache.resize(size);
      }

      unsigned int id;
      if(m_list.empty())
      {
        id = m_availableBorder++;
      }
      else
      {
        id = *m_list.begin();
        m_list.pop_front();
      }

      ResourceHandle handle(id);
      handle.add(this);

      m_objectsCache[handle.getID()] = object;

      return handle;
    }

    void updateObserver(ResourceHandle* param)
    {
      assert(param->getID() < m_availableBorder);

      m_objectsCache[param->getID()].free();

      if(param->getID() != m_availableBorder - 1)
      {
        m_list.push_front(param->getID());
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

	  CacheManager(const CacheManager&){}

    static const unsigned int m_BLOCKSIZE = 64;

    unsigned int m_availableBorder;
	  std::list<unsigned int> m_list;

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
}

#endif