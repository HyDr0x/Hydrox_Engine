#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <list>

#include "Utilities/Observer/Observer.hpp"

namespace he
{
	namespace util
	{
  template<class ParamT> class Subject
  {
  public:

    Subject(){}

    Subject(const Subject& other)
    {
      m_observer = other.m_observer;
    }

    Subject& operator=(const Subject& other)
    {
      m_observer = other.m_observer;

      return *this;
    }

    virtual ~Subject()
    {
    }

    void add(Observer<ParamT> *o)
    {
      m_observer.push_back(o);
    }

    void remove(Observer<ParamT> *o)
    {
      m_observer.remove(o);
    }

    virtual void notify(ParamT params)
    {
      for(std::list<Observer<ParamT>*>::iterator oit = m_observer.begin(); oit != m_observer.end(); oit++)
      {
        (*oit)->updateObserver(params);
      }
    }

  protected:

    std::list<Observer<ParamT>*> m_observer;
  };
	}
}

#endif
