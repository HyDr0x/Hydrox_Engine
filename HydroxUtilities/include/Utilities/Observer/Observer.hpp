#ifndef OBSERVER_H_
#define OBSERVER_H_

namespace he
{
  namespace util
  {
  template<class ParamT> class Observer
  {
  public:

    Observer(){}
    virtual ~Observer(){}

    virtual void updateObserver(ParamT params) = 0;

  private:

  };
  }
}

#endif
