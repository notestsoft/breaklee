#pragma once

#include "Base.h"
#include "AuctionDBProtocol.h"

EXTERN_C_BEGIN

enum {
#define STATEMENT(__NAME__, __QUERY__) \
	__NAME__,
#include "AuctionDBStatements.h"

	AUCTIONDB_STATEMENT_COUNT,
};

Void AuctionDBPrepareStatements(
	DatabaseRef Database
);

StatementRef AuctionDBGetStatement(
	DatabaseRef Database,
	Int32 StatementID
);

Bool AuctionDBInsertAccount(
	DatabaseRef Database,
	CString Username,
	CString Email,
	UInt8* Salt,
	Int32 SaltLength,
	UInt8* Hash,
	Int32 HashLength
);

Bool AuctionDBSelectAccountByID(
	DatabaseRef Database,
	Int64 AccountID,
	AUCTIONDB_DATA_ACCOUNT* Result
);

Bool AuctionDBSelectAccountByUsername(
	DatabaseRef Database,
	CString Username,
	AUCTIONDB_DATA_ACCOUNT* Result
);

Bool AuctionDBUpsertSession(
	DatabaseRef Database,
	Int32 AccountID,
	Bool Online,
	CString SessionKey,
	CString SessionIP
);

Bool AuctionDBSelectSession(
	DatabaseRef Database,
	Int32 AccountID,
	AUCTIONDB_DATA_SESSION* Result
);

Bool AuctionDBInsertBlacklistAccount(
	DatabaseRef Database,
	Int32 AccountID,
	CString Description
);

Bool AuctionDBInsertBlacklistAddress(
	DatabaseRef Database,
	CString AddressIP,
	CString Description
);

Bool AuctionDBSelectBlacklistByAccount(
	DatabaseRef Database,
	Int32 AccountID,
	AUCTIONDB_DATA_BLACKLIST* Result
);

Bool AuctionDBSelectBlacklistByAddress(
	DatabaseRef Database,
	CString AddressIP,
	AUCTIONDB_DATA_BLACKLIST* Result
);

Bool AuctionDBUpsertCharacter(
	DatabaseRef Database,
	Int32 AccountID,
	UInt8 WorldID,
	UInt8 CharacterCount
);

StatementRef AuctionDBSelectCharacterByID(
	DatabaseRef Database,
	Int32 AccountID
);

Bool AuctionDBSelectCharacterFetchNext(
	DatabaseRef Database,
	StatementRef Statement,
	AUCTIONDB_DATA_CHARACTER* Result
);

EXTERN_C_END
