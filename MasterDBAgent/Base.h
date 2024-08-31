#pragma once

#include <CoreLib/CoreLib.h>
#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>

#define TABLE_SCOPE_ACCOUNT "Account"
#define TABLE_SCOPE_CHARACTER "Character"

#define ReadMemory(__TYPE__, __NAME__, __COUNT__)		\
__TYPE__* __NAME__ = (__TYPE__*)Memory;					\
Int32 __NAME__##Length = sizeof(__TYPE__) * __COUNT__;	\
Memory += __NAME__##Length
