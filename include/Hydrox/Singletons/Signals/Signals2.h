#ifndef SIGNALS2_H_
#define SIGNALS2_H_

#define SIGNALS_NUM_ARGS 2
#define SIGNALS_TEMPLATE_PARAMS typename T1, typename T2
#define SIGNALS_TEMPLATE_ARGS T1, T2
#define SIGNALS_PARAMS T1 a1, T2 a2
#define SIGNALS_ARGS a1, a2

#include "Hydrox/Singletons/Signals/Signal_Template.h"

#undef SIGNALS_NUM_ARGS
#undef SIGNALS_TEMPLATE_PARAMS
#undef SIGNALS_TEMPLATE_ARGS
#undef SIGNALS_PARAMS
#undef SIGNALS_ARGS

#endif