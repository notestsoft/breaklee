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
CONFIG_PARAMETER(UInt8, GroupIndex, "LoginSvr.GroupIndex", 21)
CONFIG_PARAMETER(Bool, Maintenance, "LoginSvr.Maintenance", 0)
CONFIG_PARAMETER(Bool, CheckVersion, "LoginSvr.CheckVersion", 1)
CONFIG_PARAMETER(UInt32, ClientVersion, "LoginSvr.ClientVersion", 13209)
CONFIG_PARAMETER(UInt32, DebugVersion, "LoginSvr.DebugVersion", 0)
CONFIG_PARAMETER(UInt32, ClientMagicKey, "LoginSvr.ClientMagicKey", 646624126)
CONFIG_PARAMETER(UInt32, ServerMagicKey, "LoginSvr.ServerMagicKey", 5834620)
CONFIG_PARAMETER(UInt32, Language, "LoginSvr.Language", 0)
CONFIG_PARAMETER(UInt16, Port, "LoginSvr.Port", 38101)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "LoginSvr.MaxConnectionCount", 5)
CONFIG_PARAMETER(Bool, AutoDisconnect, "LoginSvr.AutoDisconnect", 1)
CONFIG_PARAMETER(UInt32, AutoDisconnectDelay, "LoginSvr.AutoDisconnectDelay", 120000)
CONFIG_PARAMETER(Bool, AutoCreateAccountOnLogin, "LoginSvr.AutoCreateAccountOnLogin", 1)
CONFIG_PARAMETER(Bool, EmailVerificationEnabled, "LoginSvr.EmailVerificationEnabled", 0)
CONFIG_PARAMETER(UInt64, WorldListBroadcastInterval, "LoginSvr.WorldListBroadcastInterval", 1000)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, CaptchaDataPath, "LoginSvr.CaptchaDataPath", ServerData\\Captcha)
CONFIG_PARAMETER(Bool, CaptchaVerificationEnabled, "LoginSvr.CaptchaVerificationEnabled", 0)
CONFIG_PARAMETER(Int32, LogLevel, "LoginSvr.LogLevel", 5)
CONFIG_PARAMETER(Int32, HashIterations, "LoginSvr.HashIterations", 1000)
CONFIG_END(Login)

CONFIG_BEGIN(Database)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Driver, "Database.Driver", MariaDB ODBC 3.2 Driver)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "Database.Host", localhost)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "Database.Database", Authentication)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "Database.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "Database.Password", root)
CONFIG_PARAMETER(UInt16, Port, "Database.Port", 3310)
CONFIG_PARAMETER(Bool, AutoReconnect, "Database.AutoReconnect", 1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, MigrationDataPath, "Database.MigrationDataPath", Database\\Authentication)
CONFIG_END(Database)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38161)
CONFIG_PARAMETER(UInt64, Timeout, "MasterSvr.Timeout", 1000)
CONFIG_PARAMETER(Bool, LogPackets, "MasterSvr.LogPackets", 0)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(Links)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Itemshop, "Links.Itemshop", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown1, "Links.Unknown1", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown2, "Links.Unknown2", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown3, "Links.Unknown3", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown4, "Links.Unknown4", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Guild, "Links.Guild", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, SNS, "Links.SNS", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown5, "Links.Unknown5", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown6, "Links.Unknown6", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown7, "Links.Unknown7", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown8, "Links.Unknown8", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown9, "Links.Unknown9", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown10, "Links.Unknown10", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown11, "Links.Unknown11", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown12, "Links.Unknown12", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown13, "Links.Unknown13", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown14, "Links.Unknown14", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown15, "Links.Unknown15", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown16, "Links.Unknown16", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown17, "Links.Unknown17", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown18, "Links.Unknown18", )
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown19, "Links.Unknown19", )
CONFIG_END(Links)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Int32, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Int32, WriteBufferSize, "NetLib.WriteBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Int32, PacketBufferBacklogSize, "NetLib.PacketBufferBacklogSize", 8)
CONFIG_PARAMETER(Bool, LogPackets, "NetLib.LogPackets", 0)
CONFIG_PARAMETER(Bool, UseEncryption, "NetLib.UseEncryption", 1)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
