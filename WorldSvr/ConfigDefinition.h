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

CONFIG_BEGIN(WorldSvr)
CONFIG_PARAMETER(UInt8, GroupIndex, "WorldSvr.GroupIndex", 21)
CONFIG_PARAMETER(UInt8, NodeIndex, "WorldSvr.NodeIndex", 1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "WorldSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "WorldSvr.Port", 38121)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "WorldSvr.MaxConnectionCount", 5)
CONFIG_PARAMETER(Int32, MaxPartyCount, "WorldSvr.MaxPartyCount", 5)
CONFIG_PARAMETER(UInt64, SubpasswordBanDuration, "WorldSvr.SubpasswordBanDuration", 0)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, RuntimeDataPath, "WorldSvr.RuntimeDataPath", Data)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, ServerDataPath, "WorldSvr.ServerDataPath", ServerData)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, ScriptDataPath, "WorldSvr.ScriptDataPath", Scripts)
CONFIG_PARAMETER(UInt32, MinRollDiceValue, "WorldSvr.MinRollDiceValue", 0)
CONFIG_PARAMETER(UInt32, MaxRollDiceValue, "WorldSvr.MaxRollDiceValue", 999)
CONFIG_PARAMETER(UInt32, DebugCharacter, "WorldSvr.DebugCharacter", 1)
CONFIG_PARAMETER(UInt64, DBSyncTimer, "WorldSvr.DBSyncTimer", 1000)
CONFIG_PARAMETER(UInt64, UserListBroadcastInterval, "WorldSvr.UserListBroadcastInterval", 1000)
CONFIG_PARAMETER(UInt64, WorldItemDespawnInterval, "WorldSvr.WorldItemDespawnInterval", 30000)
CONFIG_PARAMETER(Int32, NewbieSupportTimeout, "Environment.NewbieSupportTimeout", 10080)
CONFIG_PARAMETER(Int32, LogLevel, "WorldSvr.LogLevel", 5)
CONFIG_END(WorldSvr)

CONFIG_BEGIN(Environment)
CONFIG_PARAMETER(UInt16, MaxLevel, "Environment.MaxLevel", 200)
CONFIG_PARAMETER(UInt8, DummyEnabled, "Environment.DummyEnabled", 1)
CONFIG_PARAMETER(UInt8, CashshopEnabled, "Environment.CashshopEnabled", 1)
CONFIG_PARAMETER(UInt8, NetCafePointEnabled, "Environment.NetCafePointEnabled", 1)
CONFIG_PARAMETER(UInt16, MinChatLevel, "Environment.MinChatLevel", 10)
CONFIG_PARAMETER(UInt16, MinShoutLevel, "Environment.MinShoutLevel", 5)
CONFIG_PARAMETER(UInt16, MinShoutSkillRank, "Environment.MinShoutSkillRank", 0)
CONFIG_PARAMETER(UInt64, MaxInventoryCurrency, "Environment.MaxInventoryCurrency", 1000000000000)
CONFIG_PARAMETER(UInt64, MaxWarehouseCurrency, "Environment.MaxWarehouseCurrency", 1000000000000)
CONFIG_PARAMETER(UInt64, MaxAuctionCurrency, "Environment.MaxAuctionCurrency", 900000000000)
CONFIG_PARAMETER(Int64, MaxHonorPoint, "Environment.MaxHonorPoint", 90000000000)
CONFIG_PARAMETER(Int64, MinHonorPoint, "Environment.MinHonorPoint", -2000000000)
CONFIG_PARAMETER(Bool, IsPKEnabled, "Environment.IsPKEnabled", 0)
CONFIG_PARAMETER(Bool, IsPremiumEnabled, "Environment.IsPremiumEnabled", 0)
CONFIG_PARAMETER(Bool, IsWarEnabled, "Environment.IsWarEnabled", 0)
CONFIG_PARAMETER(Bool, IsRestrictedEnabled, "Environment.IsRestrictedEnabled", 0)
CONFIG_PARAMETER(Bool, IsEventEnabled, "Environment.IsEventEnabled", 0)
CONFIG_PARAMETER(Bool, IsNoviceEnabled, "Environment.IsNoviceEnabled", 0)
CONFIG_PARAMETER(Bool, IsOnly2FAEnabled, "Environment.IsOnly2FAEnabled", 0)
CONFIG_PARAMETER(Bool, IsRaidBossEnabled, "Environment.IsRaidBossEnabled", 1)
CONFIG_PARAMETER(Bool, IsNationEnabled, "Environment.IsNationEnabled", 0)
CONFIG_PARAMETER(Int32, FieldBossRaidScheduleWeekCount, "Environment.FieldBossRaidScheduleWeekCount", 12)
CONFIG_PARAMETER(Bool, IsSkillRankUpLimitEnabled, "Environment.IsSkillRankUpLimitEnabled", 0)
CONFIG_PARAMETER(Bool, IsCostumeWarehouseEnabled, "Environment.IsCostumeWarehouseEnabled", 1)
CONFIG_END(Environment)

CONFIG_BEGIN(ChatSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "ChatSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "ChatSvr.Port", 38170)
CONFIG_END(ChatSvr)

CONFIG_BEGIN(AuctionSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuctionSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "AuctionSvr.Port", 38160)
CONFIG_END(AuctionSvr)

CONFIG_BEGIN(PartySvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "PartySvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "PartySvr.Port", 38190)
CONFIG_END(PartySvr)

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
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x7FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x7FFFF)
CONFIG_PARAMETER(Bool, LogPackets, "NetLib.LogPackets", 0)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
