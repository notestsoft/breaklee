
#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"
#include "IPCProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)
#endif

CLIENT_PROTOCOL(S2C, NFY_ERROR, DEFAULT, 7,
    UInt16 ErrorCommand;
    UInt16 ErrorSubCommand;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, CONNECT, DEFAULT, 101,
    UInt32 AuthKey;
)

CLIENT_PROTOCOL(S2C, CONNECT, DEFAULT, 101,
    UInt32 XorKey;
    UInt32 AuthKey;
    UInt16 ConnectionID;
    UInt16 XorKeyIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, AUTH_ACCOUNT, DEFAULT, 102,
    UInt32 CharacterIndex;
    UInt8 WorldServerIndex;
    UInt32 Unknown1;
    UInt64 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_AUTH_ACCOUNT_CATEGORY_INDEX,
    UInt16 Unknown1;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(S2C, AUTH_ACCOUNT, DEFAULT, 102,
    UInt8 Status;
    UInt8 CategoryCount1;
    UInt8 CategoryCount2;
    Int32 Unknown1;
    Int32 Unknown2;
    // S2C_AUTH_ACCOUNT_CATEGORY_INDEX Data[CategoryCount1 + CategoryCount2];
)

CLIENT_PROTOCOL(C2S, SERVER_ENVIRONMENT, DEFAULT, 103,
)

CLIENT_PROTOCOL(S2C, SERVER_ENVIRONMENT, DEFAULT, 103,
    Int16 MaxSearchResultCount;
    UInt16 BuyListCooldown;
    UInt16 SellListCooldown;
    UInt16 CancelSlotCooldown;
    Int16 NoCancelCommissionTimeout;
    Int16 NoServerResponseTimeout;
    UInt8 UseForceGem;
    Int16 CommissionRateForSell;
    Int16 CommissionRateForSellForceGem;
    Char Name[21];
    UInt8 AlertAveragePrice;
    Int16 EntrySlotCount;
    Int16 EntrySlotDurationInMinutes;
    Int16 EntrySlotMaxQuantity;
    Int16 DefaultSlotCount;
    Int16 PremiumSlotCount;
    UInt32 Unknown1;
    Int16 Unknown2[16];
)

CLIENT_PROTOCOL(C2S, DISCONNECT, DEFAULT, 104,
)

CLIENT_PROTOCOL(C2S, SEARCH, DEFAULT, 111,
    UInt8 CategoryIndex2;
    UInt16 CategoryIndex3;
    UInt8 CategoryIndex4;
    UInt16 CategoryIndex5;
    UInt16 SortOrder;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SEARCH_RESULT_SLOT,
    UInt8 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemOptionExtended;
    Int16 StackSize;
    UInt8 PriceType;
    Int64 Price;
    UInt32 AccountID;
    UInt8 NameLength;
    // Char Name[0];
    // UInt8 GuildNameLength;
    // Char GuildName[];
)

CLIENT_PROTOCOL(S2C, SEARCH, DEFAULT, 111,
    UInt8 Unknown1;
    Int32 Unknown2;
    UInt8 Unknown3;
    Int16 ResultCount;
    S2C_DATA_SEARCH_RESULT_SLOT ResultSlots[0];
)

CLIENT_PROTOCOL(C2S, GET_ITEM_LIST, DEFAULT, 112,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_ITEM_LIST_INDEX,
    UInt8 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 Unknown1;
    UInt16 ItemCount;
    UInt8 ItemPriceType;
    UInt64 ItemPrice;
    UInt16 SoldItemCount;
    UInt64 ExpirationDate;
    UInt64 RegistrationDate;
)

CLIENT_PROTOCOL(S2C, GET_ITEM_LIST, DEFAULT, 112,
    UInt8 Result;
    UInt8 ItemCount;
    // S2C_DATA_GET_ITEM_LIST_INDEX Items[0];
)

CLIENT_PROTOCOL(C2S, GUILD_INVENTORY_GET_ITEM_LIST, DEFAULT, 115,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, GUILD_INVENTORY_GET_ITEM_LIST, DEFAULT, 115,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, GUILD_INVENTORY_CHANGE_CURRENCY, DEFAULT, 116,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, GUILD_INVENTORY_CHANGE_ITEM, DEFAULT, 117,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(C2S, GUILD_INVENTORY_MERGE_ITEM, DEFAULT, 118,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, GUILD_INVENTORY_MERGE_ITEM, DEFAULT, 118,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(C2S, GUILD_INVENTORY_GET_HISTORY, DEFAULT, 119,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, GUILD_INVENTORY_GET_HISTORY, DEFAULT, 119,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(C2S, GET_ITEM_AVERAGE_PRICE, DEFAULT, 2001,
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 Unknown1;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, GET_ITEM_AVERAGE_PRICE, DEFAULT, 2001,
    UInt64 Price;
)

CLIENT_PROTOCOL(C2S, GET_BOOKMARK, DEFAULT, 2002,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_BOOKMARK_SLOT,
    UInt8 SlotIndex;
    UInt16 CategoryIndex[5];
    UInt8 SubCategoryIndex[2];
    Char Description[MAX_AUCTION_BOOKMARK_DESCRIPTION_LENGTH];
)

CLIENT_PROTOCOL(S2C, GET_BOOKMARK, DEFAULT, 2002,
    UInt8 Result;
    UInt8 Count;
    //C2S_DATA_GET_BOOKMARK_SLOT Bookmarks[0];
)

CLIENT_PROTOCOL(C2S, SET_BOOKMARK, DEFAULT, 2003,
    S2C_DATA_GET_BOOKMARK_SLOT Bookmark;
)

CLIENT_PROTOCOL(S2C, SET_BOOKMARK, DEFAULT, 2003,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, DELETE_BOOKMARK, DEFAULT, 2004,
    UInt8 SlotIndex;
)

CLIENT_PROTOCOL(S2C, DELETE_BOOKMARK, DEFAULT, 2004,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, GET_ITEM_MINIMUM_PRICE, DEFAULT, 2005,
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 Unknown1;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, GET_ITEM_MINIMUM_PRICE, DEFAULT, 2005,
    UInt8 Result;
    UInt64 Price;
)

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
