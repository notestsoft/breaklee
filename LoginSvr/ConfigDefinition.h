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

CONFIG_BEGIN(Login)
CONFIG_PARAMETER(Bool, Maintenance, "LoginSvr.Maintenance", 0)
CONFIG_PARAMETER(Bool, CheckVersion, "LoginSvr.CheckVersion", 1)
CONFIG_PARAMETER(UInt32, ClientVersion, "LoginSvr.ClientVersion", 13155)
CONFIG_PARAMETER(UInt32, DebugVersion, "LoginSvr.DebugVersion", 0)
CONFIG_PARAMETER(UInt32, ClientMagicKey, "LoginSvr.ClientMagicKey", 974070038)
CONFIG_PARAMETER(UInt32, ServerMagicKey, "LoginSvr.ServerMagicKey", 5834620)
CONFIG_PARAMETER(UInt16, Port, "LoginSvr.Port", 38101)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "LoginSvr.MaxConnectionCount", 64)
CONFIG_PARAMETER(Bool, AutoDisconnect, "LoginSvr.AutoDisconnect", 1)
CONFIG_PARAMETER(UInt32, AutoDisconnectDelay, "LoginSvr.AutoDisconnectDelay", 120000)
CONFIG_PARAMETER(Bool, AutoCreateAccountOnLogin, "LoginSvr.AutoCreateAccountOnLogin", 1)
CONFIG_PARAMETER(Bool, EmailVerificationEnabled, "LoginSvr.EmailVerificationEnabled", 0)
CONFIG_PARAMETER(UInt64, WorldListBroadcastInterval, "LoginSvr.WorldListBroadcastInterval", 1000)
CONFIG_END(Login)

CONFIG_BEGIN(AuthDB)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuthDB.Host", 127.0.0.1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "AuthDB.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "AuthDB.Password", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "AuthDB.Database", auth)
CONFIG_PARAMETER(UInt16, Port, "AuthDB.Port", 3310)
CONFIG_PARAMETER(Bool, AutoReconnect, "AuthDB.AutoReconnect", 1)
CONFIG_END(AuthDB)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38161)
CONFIG_PARAMETER(UInt64, Timeout, "MasterSvr.Timeout", 1000)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(Links)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Itemshop, "Links.Itemshop", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown1, "Links.Unknown1", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown2, "Links.Unknown2", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown3, "Links.Unknown3", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown4, "Links.Unknown4", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Guild, "Links.Guild", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, SNS, "Links.SNS", )
CONFIG_END(Links)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x1FFFF)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
