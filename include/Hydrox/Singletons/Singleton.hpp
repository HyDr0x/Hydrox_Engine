#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <map>
#include <assert.h>
#include <string>

namespace he
{
  template<class T> class Singleton
  {
  public:

    static T* getInstance()
    {
      return m_instance;
    }

  protected:

    Singleton(){}
    Singleton(const Singleton&){}
    Singleton& operator=(const Singleton&){ return *this; }
    ~Singleton(){} 

    static T* m_instance;
  };

  template<class T> T* Singleton<T>::m_instance = nullptr;
}

#endif