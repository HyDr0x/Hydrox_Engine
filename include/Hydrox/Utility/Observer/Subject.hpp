#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <list>

#include "Hydrox/Utility/Observer/Observer.hpp"

namespace he
{
  template<class ParamT> class Subject
  {
  public:

    Subject(){}

    Subject(const Subject& o)
    {
      m_observer = o.m_observer;
    }

    Subject& operator=(const Subject& o)
    {
      m_observer = o.m_observer;

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

#endif