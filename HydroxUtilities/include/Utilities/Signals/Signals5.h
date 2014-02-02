#ifndef SIGNALS5_H_
#define SIGNALS5_H_

#define SIGNALS_NUM_ARGS 5
#define SIGNALS_TEMPLATE_PARAMS typename T1, typename T2, typename T3, typename T4, typename T5
#define SIGNALS_TEMPLATE_ARGS T1, T2, T3, T4, T5
#define SIGNALS_PARAMS T1 a1, T2 a2, T3 a3, T4 a4, T5 a5
#define SIGNALS_ARGS a1, a2, a3, a4, a5

#include "Utilities/Signals/Signal_Template.h"

#undef SIGNALS_NUM_ARGS
#undef SIGNALS_TEMPLATE_PARAMS
#undef SIGNALS_TEMPLATE_ARGS
#undef SIGNALS_PARAMS
#undef SIGNALS_ARGS

#endif
