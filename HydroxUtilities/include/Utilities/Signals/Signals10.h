#ifndef SIGNALS10_H_
#define SIGNALS10_H_

#define SIGNALS_NUM_ARGS 10
#define SIGNALS_TEMPLATE_PARAMS typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10
#define SIGNALS_TEMPLATE_ARGS T1, T2, T3, T4, T5, T6, T7, T8, T9, T10
#define SIGNALS_PARAMS T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9, T10 a10
#define SIGNALS_ARGS a1, a2, a3, a4, a5, a6, a7, a8, a9, a10

#include "Utilities/Signals/Signal_Template.h"

#undef SIGNALS_NUM_ARGS
#undef SIGNALS_TEMPLATE_PARAMS
#undef SIGNALS_TEMPLATE_ARGS
#undef SIGNALS_PARAMS
#undef SIGNALS_ARGS

#endif
