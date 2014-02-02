#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <map>
#include <assert.h>
#include <string>

namespace he
{
	namespace util
	{
    template<class T> class Singleton
    {
    public:

      static T* getInstance()
      {
        if(m_instance == nullptr) m_instance = new T;
        return m_instance;
      }

      static void deleteInstance()
      {
        assert(m_instance != nullptr);
        delete m_instance;
      }

    protected:

      Singleton(){}
      virtual ~Singleton(){} 

    private:

      Singleton(const Singleton&);
      Singleton& operator=(const Singleton&);

      static T* m_instance;
    };

  template<class T> T* Singleton<T>::m_instance = nullptr;
	}
}

#endif
