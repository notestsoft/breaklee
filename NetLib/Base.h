#pragma once

#include <CoreLib/CoreLib.h>
#include <uv.h>

#ifdef _WIN32
typedef SOCKET SocketHandle;
#else
typedef Int32 SocketHandle;
#endif

typedef struct sockaddr_in SocketAddress;
typedef struct sockaddr* SocketAddressRef;
