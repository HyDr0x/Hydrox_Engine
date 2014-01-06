#ifndef SIGNALS3_H_
#define SIGNALS3_H_

#define SIGNALS_NUM_ARGS 3
#define SIGNALS_TEMPLATE_PARAMS typename T1, typename T2, typename T3
#define SIGNALS_TEMPLATE_ARGS T1, T2, T3
#define SIGNALS_PARAMS T1 a1, T2 a2, T3 a3
#define SIGNALS_ARGS a1, a2, a3

#include "Hydrox/Singletons/Signals/Signal_Template.h"

#undef SIGNALS_NUM_ARGS
#undef SIGNALS_TEMPLATE_PARAMS
#undef SIGNALS_TEMPLATE_ARGS
#undef SIGNALS_PARAMS
#undef SIGNALS_ARGS

#endif
