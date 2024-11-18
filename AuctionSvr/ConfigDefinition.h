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

CONFIG_BEGIN(AuctionSvr)
CONFIG_PARAMETER(UInt8, GroupIndex, "AuctionSvr.GroupIndex", 21)
CONFIG_PARAMETER(UInt16, Port, "AuctionSvr.Port", 38160)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "AuctionSvr.MaxConnectionCount", 5)
CONFIG_PARAMETER(Int32, LogLevel, "AuctionSvr.LogLevel", 5)
CONFIG_END(AuctionSvr)

// TODO: Move Environment to database

CONFIG_BEGIN(Environment)
CONFIG_PARAMETER(Int16, MaxSearchResultCount, "Environment.MaxSearchResultCount", 100)
CONFIG_PARAMETER(Int16, BuyListCooldown, "Environment.BuyListCooldown", 1)
CONFIG_PARAMETER(Int16, SellListCooldown, "Environment.SellListCooldown", 1)
CONFIG_PARAMETER(Int16, CancelSlotCooldown, "Environment.CancelSlotCooldown", 0)
CONFIG_PARAMETER(Int16, NoCancelCommissionTimeout, "Environment.NoCancelCommissionTimeout", 300)
CONFIG_PARAMETER(Int16, NoServerResponseTimeout, "Environment.NoServerResponseTimeout", 20)
CONFIG_PARAMETER(Int16, UseForceGem, "Environment.UseForceGem", 1)
CONFIG_PARAMETER(Int16, CommissionRateForSell, "Environment.CommissionRateForSell", 400)
CONFIG_PARAMETER(Int16, CommissionRateForSellForceGem, "Environment.CommissionRateForSellForceGem", 400)
CONFIG_PARAMETER_ARRAY(Char, 21, Name, "Environment.Name", [BREAKLEE])
CONFIG_PARAMETER(Int16, AlertAveragePrice, "Environment.AlertAveragePrice", 25)
CONFIG_PARAMETER(Int16, EntrySlotCount, "Environment.EntrySlotCount", 3)
CONFIG_PARAMETER(Int16, DefaultSlotCount, "Environment.DefaultSlotCount", 3)
CONFIG_PARAMETER(Int16, PremiumSlotCount, "Environment.PremiumSlotCount", 27)
CONFIG_END(Environment)

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
