#pragma once

#include "Base.h"

#pragma pack(push, 1)

// Forward Declarations

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__) \
typedef struct CONCAT(_RTData, __NAME__) *CONCAT(RTData, __NAME__##Ref);

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__) \
typedef struct CONCAT(_RTData, __NAME__) *CONCAT(RTData, __NAME__##Ref);

#include "Macro.h"

// Type Declarations

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__) \
struct CONCAT(_RTData, __NAME__) {

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__) \
Int32 CONCAT(__NAME__, Count);										  \
struct CONCAT(_RTData, __NAME__) {

#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__) \
} *CONCAT(__NAME__, List);

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__) \
	__TYPE__ __NAME__;

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__) \
	Int32 CONCAT(__NAME__, Count);															 \
	__TYPE__ __NAME__[__COUNT__];

#define RUNTIME_DATA_TYPE_END(__NAME__) \
};

#include "Macro.h"

#pragma pack(pop)
