template<typename R SIGNALS_COMMA_IF_ARGUMENTS 
  SIGNALS_TEMPLATE_PARAMS
> 
class SIGNALS_DELEGATE
{
public:

  virtual ~SIGNALS_DELEGATE()=0
  {}

  virtual R invoke(SIGNALS_PARAMS)=0;

  virtual bool operator==(SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>* obj)=0;

private:

};
