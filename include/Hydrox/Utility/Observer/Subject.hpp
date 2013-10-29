#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <list>

#include "Hydrox/Utility/Observer/Observer.hpp"

template<class ParamT> class Subject
{
public:

  Subject(){}
  virtual ~Subject()
  {
    m_observer.clear();
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

private:

  std::list<Observer<ParamT>*> m_observer;
};


#endif