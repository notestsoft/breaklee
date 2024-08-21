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

CONFIG_BEGIN(MasterDBAgent)
CONFIG_PARAMETER(UInt8, GroupIndex, "MasterDBAgent.GroupIndex", 21)
CONFIG_PARAMETER(Int32, LogLevel, "WorldSvr.LogLevel", 5)
CONFIG_END(MasterDBAgent)

CONFIG_BEGIN(Database)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Driver, "Database.Driver", MariaDB ODBC 3.2 Driver)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "Database.Host", localhost)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "Database.Database", GameServer01)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "Database.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "Database.Password", root)
CONFIG_PARAMETER(UInt16, Port, "Database.Port", 3312)
CONFIG_PARAMETER(Bool, AutoReconnect, "Database.AutoReconnect", 1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, MigrationDataPath, "Database.MigrationDataPath", Database\\GameServer01)
CONFIG_END(Database)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38161)
CONFIG_PARAMETER(UInt64, Timeout, "MasterSvr.Timeout", 1000)
CONFIG_PARAMETER(Bool, LogPackets, "MasterSvr.LogPackets", 0)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x7FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x7FFFF)
CONFIG_PARAMETER(Bool, LogPackets, "NetLib.LogPackets", 0)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
