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

      inline static T& getInstance()
      {
        if(m_instance == nullptr) m_instance = new T;
        return *m_instance;
      }

      inline static void deleteInstance()
      {
        delete m_instance;
      }       

    private:

      Singleton(){}
      virtual ~Singleton(){}
      Singleton(const Singleton<T>&);
      Singleton& operator=(const Singleton<T>&);

      static T* m_instance;
    };

  template<class T> T* Singleton<T>::m_instance = nullptr;
  }
}

#endif
