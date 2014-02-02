#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <list>

#include "Utilities/Signals/FunctionTraits.h"

#define SIGNALS_JOIN(arg1,arg2) SIGNALS_JOIN2(arg1,arg2)//necassarily evil
#define SIGNALS_JOIN2(arg1,arg2) arg1##arg2

#include "Utilities/Signals/Signals0.h"
#include "Utilities/Signals/Signals1.h"
#include "Utilities/Signals/Signals2.h"
#include "Utilities/Signals/Signals3.h"
#include "Utilities/Signals/Signals4.h"
#include "Utilities/Signals/Signals5.h"
#include "Utilities/Signals/Signals6.h"
#include "Utilities/Signals/Signals7.h"
#include "Utilities/Signals/Signals8.h"
#include "Utilities/Signals/Signals9.h"
#include "Utilities/Signals/Signals10.h"

#undef SIGNALS_JOIN
#undef SIGNALS_JOIN2

namespace he
{
	namespace util
	{
    ////////////////////////////////////Small Signal Wrapper////////////////////////////////
    template<int arity, typename trait, typename Signature> struct getSignalImpl;//dummy struct, shouldn't be instanciated
    //{
    //	typedef Signal0<typename trait::returnType> type;//dummy typedef
    //};

    template<typename trait, typename Signature> struct getSignalImpl<0,trait,Signature>
    {
	    typedef Signal0<typename trait::returnType> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<1,trait,Signature>
    {
	    typedef Signal1<typename trait::returnType,typename trait::arg1Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<2,trait,Signature>
    {
	    typedef Signal2<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<3,trait,Signature>
    {
	    typedef Signal3<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<4,trait,Signature>
    {
	    typedef Signal4<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<5,trait,Signature>
    {
	    typedef Signal5<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<6,trait,Signature>
    {
	    typedef Signal6<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type, typename trait::arg6Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<7,trait,Signature>
    {
	    typedef Signal7<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type, typename trait::arg6Type, typename trait::arg7Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<8,trait,Signature>
    {
	    typedef Signal8<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type, typename trait::arg6Type, typename trait::arg7Type, typename trait::arg8Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<9,trait,Signature>
    {
	    typedef Signal9<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type, typename trait::arg6Type, typename trait::arg7Type, typename trait::arg8Type, typename trait::arg9Type> type;
    };

    template<typename trait, typename Signature> struct getSignalImpl<10,trait,Signature>
    {
	    typedef Signal10<typename trait::returnType,typename trait::arg1Type, typename trait::arg2Type, typename trait::arg3Type, typename trait::arg4Type, typename trait::arg5Type, typename trait::arg6Type, typename trait::arg7Type, typename trait::arg8Type, typename trait::arg9Type, typename trait::arg10Type> type;
    };
    ////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////Savable Signal Base Class////////////////////////////////
    class signal_save {public: virtual ~signal_save()=0{}};
    ////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////Small Interface////////////////////////////////
    template<
	    typename Signature, //(R (T1, T2, ..., TN)
	    typename trait=FunctionTrait<Signature>
    > 
    class Signal : 
	    public getSignalImpl<trait::arity,trait,Signature>::type, public signal_save
    {

    };
    ////////////////////////////////////////////////////////////////////////
	}
}

#endif
