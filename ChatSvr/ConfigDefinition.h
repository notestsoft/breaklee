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

CONFIG_BEGIN(ChatSvr)
CONFIG_PARAMETER(UInt8, GroupIndex, "ChatSvr.GroupIndex", 21)
CONFIG_PARAMETER(UInt16, Port, "ChatSvr.Port", 38170)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "ChatSvr.MaxConnectionCount", 5)
CONFIG_PARAMETER(Int32, LogLevel, "ChatSvr.LogLevel", 5)
CONFIG_END(ChatSvr)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38161)
CONFIG_PARAMETER(UInt64, Timeout, "MasterSvr.Timeout", 1000)
CONFIG_PARAMETER(Bool, LogPackets, "MasterSvr.LogPackets", 0)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Int32, ReadBufferSize, "NetLib.ReadBufferSize", 0x0FFFF)
CONFIG_PARAMETER(Int32, WriteBufferSize, "NetLib.WriteBufferSize", 0x0FFFF)
CONFIG_PARAMETER(Int32, PacketBufferBacklogSize, "NetLib.PacketBufferBacklogSize", 8)
CONFIG_PARAMETER(Bool, LogPackets, "NetLib.LogPackets", 0)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
