#pragma once

#include "Base.h"

#ifdef _WIN32
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

Void PlatformLoadSocketLibrary();

Void PlatformUnloadSocketLibrary();

Bool PlatformSetSocketIOBlocking(
    SocketHandle Handle,
    Bool Blocking
);

Bool PlatformSocketAccept(
    SocketHandle Handle,
    SocketAddressRef Address,
    UInt32 Length,
    SocketHandle* Result
);

Int32 PlatformSocketConnect(
    SocketHandle Handle,
    SocketAddressRef Address,
    Int32 Length
);

Bool PlatformSocketSelect(
    SocketHandle Handle,
    Timestamp Timeout
);

Bool PlatformSocketRecv(
    SocketHandle Handle,
    UInt8* Buffer,
    Int64 Length,
    Int64* ReadLength
);

Void PlatformSocketClose(
    SocketHandle Handle
);

Void PlatformSocketAddressToString(
    SocketAddress Address,
    Int32 Length,
    CString Buffer
);
