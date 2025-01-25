#pragma once

#include "Base.h"
#include "UIList.h"
#include "UIView.h"

EXTERN_C_BEGIN

enum {
	UI_ITEM_LIST_EBM,
	UI_ITEM_LIST_EFX,
	UI_ITEM_LIST_EPS,
	UI_ITEM_LIST_MCL,
	UI_ITEM_LIST_QD1,
	UI_ITEM_LIST_MD1,
	UI_ITEM_LIST_MD2,
	UI_ITEM_LIST_MOB,
	UI_ITEM_LIST_COUNT,
};

struct _ControlUIState {
	Char ToolbarText[UI_TEXT_LENGTH];
	Int32 ToolbarActiveIndex;
	Char SearchBoxText[UI_TEXT_LENGTH];
	Bool SearchBoxEditMode;
	Char DungeonListItems[RAYGUI_TEXTSPLIT_MAX_TEXT_SIZE];
	Int DungeonListTargets[1024];
	Int32 DungeonListIndex;
	Int32 DungeonListActive;
	CString DungeonListActiveName;

	UIListRef ItemLists[UI_ITEM_LIST_COUNT];
};
typedef struct _ControlUIState* ControlUIStateRef;

Void ControlUIInit(
	EditorContextRef Context,
	ControlUIStateRef State
);

Void ControlUIDeinit(
	EditorContextRef Context,
	ControlUIStateRef State
);

Void ControlUIDraw(
	EditorContextRef Context,
	ControlUIStateRef State
);

EXTERN_C_END