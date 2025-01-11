#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _UIListItem* UIListItemRef;
typedef struct _UIList* UIListRef;

UIListRef UIListCreate(
	AllocatorRef Allocator,
	Int32 ItemDataLength
);

Void UIListDestroy(
	UIListRef List
);

UIListItemRef UIListAppendItem(
	UIListRef List,
	CString Name,
	UInt8* Data,
	Int32 DataLength
);

UInt8* UIListItemGetData(
	UIListItemRef ListItem
);

Void UIListFilter(
	UIListRef List,
	CString SearchTerm
);

Bool UIListDraw(
	EditorContextRef Context,
	UIListRef List,
	Rectangle Frame,
	UIListItemRef* Selection
);

EXTERN_C_END