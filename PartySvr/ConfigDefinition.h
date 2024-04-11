#ifndef CONFIG_BEGIN
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(PartySvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "PartySvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "PartySvr.Port", 38190)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "PartySvr.MaxConnectionCount", 8192)
CONFIG_PARAMETER(Int32, MaxPartyCount, "PartySvr.MaxPartyCount", 4096)
CONFIG_END(PartySvr)

CONFIG_BEGIN(WorldSvr)
CONFIG_PARAMETER(UInt16, Port, "WorldSvr.Port", 38191)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "WorldSvr.MaxConnectionCount", 32)
CONFIG_END(WorldSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x7FFFF)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
