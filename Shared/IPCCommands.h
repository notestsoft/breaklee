#ifndef IPC_A2W_COMMAND
#define IPC_A2W_COMMAND(__NAME__)
#endif

#ifndef IPC_A2D_COMMAND
#define IPC_A2D_COMMAND(__NAME__)
#endif

#ifndef IPC_C2W_COMMAND
#define IPC_C2W_COMMAND(__NAME__)
#endif

#ifndef IPC_C2D_COMMAND
#define IPC_C2D_COMMAND(__NAME__)
#endif

#ifndef IPC_D2A_COMMAND
#define IPC_D2A_COMMAND(__NAME__)
#endif

#ifndef IPC_D2C_COMMAND
#define IPC_D2C_COMMAND(__NAME__)
#endif

#ifndef IPC_D2W_COMMAND
#define IPC_D2W_COMMAND(__NAME__)
#endif

#ifndef IPC_L2W_COMMAND
#define IPC_L2W_COMMAND(__NAME__)
#endif

#ifndef IPC_L2M_COMMAND
#define IPC_L2M_COMMAND(__NAME__)
#endif

#ifndef IPC_M2W_COMMAND
#define IPC_M2W_COMMAND(__NAME__)
#endif

#ifndef IPC_M2L_COMMAND
#define IPC_M2L_COMMAND(__NAME__)
#endif

#ifndef IPC_P2W_COMMAND
#define IPC_P2W_COMMAND(__NAME__)
#endif

#ifndef IPC_W2A_COMMAND
#define IPC_W2A_COMMAND(__NAME__)
#endif

#ifndef IPC_W2C_COMMAND
#define IPC_W2C_COMMAND(__NAME__)
#endif

#ifndef IPC_W2D_COMMAND
#define IPC_W2D_COMMAND(__NAME__)
#endif

#ifndef IPC_W2L_COMMAND
#define IPC_W2L_COMMAND(__NAME__)
#endif

#ifndef IPC_W2P_COMMAND
#define IPC_W2P_COMMAND(__NAME__)
#endif

#ifndef IPC_W2M_COMMAND
#define IPC_W2M_COMMAND(__NAME__)
#endif

#ifndef IPC_W2W_COMMAND
#define IPC_W2W_COMMAND(__NAME__)
#endif

#ifndef IPC_N2M_COMMAND
#define IPC_N2M_COMMAND(__NAME__)
#endif

#ifndef IPC_M2N_COMMAND
#define IPC_M2N_COMMAND(__NAME__)
#endif

#define IPC_PROTOCOL(__NAMESPACE__, __NAME__, __BODY__) \
IPC_ ## __NAMESPACE__ ## _COMMAND(__NAME__)
#include "IPCProtocolDefinition.h"

#undef IPC_A2W_COMMAND
#undef IPC_A2D_COMMAND
#undef IPC_C2W_COMMAND
#undef IPC_C2D_COMMAND
#undef IPC_D2A_COMMAND
#undef IPC_D2C_COMMAND
#undef IPC_D2W_COMMAND
#undef IPC_L2W_COMMAND
#undef IPC_L2M_COMMAND
#undef IPC_M2W_COMMAND
#undef IPC_M2L_COMMAND
#undef IPC_P2W_COMMAND
#undef IPC_W2A_COMMAND
#undef IPC_W2C_COMMAND
#undef IPC_W2D_COMMAND
#undef IPC_W2L_COMMAND
#undef IPC_W2P_COMMAND
#undef IPC_W2M_COMMAND
#undef IPC_W2W_COMMAND
#undef IPC_N2M_COMMAND
#undef IPC_M2N_COMMAND