#include "Context.h"
#include "ControlUI.h"

Void UnloadArchive(
	EditorContextRef Context,
	ControlUIStateRef State
) {
	if (!Context->Archive) return;

	switch (Context->ArchiveItemType) {
	case UI_ITEM_LIST_EBM: {
		EBMArchiveDestroy((EBMArchiveRef)Context->Archive);
		break;
	}

	case UI_ITEM_LIST_EFX: {
		break;
	}

	case UI_ITEM_LIST_EPS: {
		break;
	}

	case UI_ITEM_LIST_MCL: {
		MCLArchiveDestroy((MCLArchiveRef)Context->Archive);
		break;
	}

	default:
		break;
	}

	Context->ArchiveItemType = -1;
	Context->Archive = NULL;
}

Void LoadArchive(
	EditorContextRef Context,
	ControlUIStateRef State,
	CString FilePath,
	Int32 ArchiveItemType
) {
	UnloadArchive(Context, State);

	switch (ArchiveItemType) {
	case UI_ITEM_LIST_EBM: {
		EBMArchiveRef Archive = EBMArchiveCreate(Context->Allocator);
		if (EBMArchiveLoadFromFile(Archive, FilePath)) {
			Context->ArchiveItemType = ArchiveItemType;
			Context->Archive = Archive;
			EBMArchiveSetupCamera(Context, Archive);
		}
		else {
			EBMArchiveDestroy(Archive);
		}

		break;
	}

	case UI_ITEM_LIST_EFX: {
		break;
	}

	case UI_ITEM_LIST_EPS: {
		break;
	}

	case UI_ITEM_LIST_MCL: {
		MCLArchiveRef Archive = MCLArchiveCreate(Context->Allocator);
		if (MCLArchiveLoadFromFile(Context, Archive, FilePath)) {
			Context->ArchiveItemType = ArchiveItemType;
			Context->Archive = Archive;
		}
		else {
			MCLArchiveDestroy(Archive);
		}

		break;
	}

	default:
		break;
	}
}

Void ControlUIUpdateDungeonListItems(
	EditorContextRef Context,
	ControlUIStateRef State,
	CString SearchQuery
) {
	Int Offset = 0;
	State->DungeonListItems[Offset] = '\0';
	Int IndexOffset = 0;
	for (Int Index = 0; Index < ArrayGetElementCount(Context->MissionDungeons); Index += 1) {
		MissionDungeonDataRef Data = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons, Index);
		State->DungeonListTargets[Index] = IndexOffset;

		Int DungeonIndex = Data->QDungeonIdx;
		CString Name = DictionaryLookup(Context->MissionDungeonNames, &DungeonIndex);
		if (Name) {
			if (strlen(SearchQuery) > 0 && !strstr(Name, SearchQuery)) continue;

			sprintf(&State->DungeonListItems[Offset], "%s;", Name);
			Offset += strlen(Name) + 1;
			State->DungeonListItems[Offset] = '\0';
		}
		else {
			Char TempName[64] = { 0 };
			sprintf(TempName, "dungeon_%d;", Data->QDungeonIdx);
			if (strlen(SearchQuery) > 0 && !strstr(TempName, SearchQuery)) continue;

			sprintf(&State->DungeonListItems[Offset], "%s;", TempName);
			Offset += strlen(TempName) + 1;
			State->DungeonListItems[Offset] = '\0';
		}

		IndexOffset += 1;
	}
}

Void OnFilesList(
	CString FileName,
	CString FilePath,
	Void* UserData
) {
	UIListRef List = (UIListRef)UserData;
	UIListAppendItem(List, FileName, FilePath, strlen(FilePath) + 1);
}

Void ControlUIInit(
	EditorContextRef Context,
	ControlUIStateRef State
) {
	/*
	local_2378 = ".ebs";
    local_2338 = ".ech";
    local_22f8 = ".ewt";
    local_22b8 = ".eps";
    local_2278 = ".ebd";
    local_2238 = ".egs";
    local_21f8 = ".efx";
    local_21b8 = ".efk";
    local_2178 = ".els";
    local_2138 = ".elt";
    local_20f8 = ".edt";
    local_20b8 = ".gls";
    local_2078 = ".mfx";
	*/
	CStringCopySafe(State->ToolbarText, sizeof(State->ToolbarText), "ebm;efx;eps;mcl;qd1;md1;md2");
	State->ToolbarActiveIndex = 0;
	ControlUIUpdateDungeonListItems(Context, State, State->SearchBoxText);
	State->DungeonListActiveName = NULL;

	State->EbmItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->EfxItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->EpsItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->MclItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->Qd1ItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->Md1ItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->Md2ItemList = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	State->IndexToItemList[UI_ITEM_LIST_EBM] = State->EbmItemList;
	State->IndexToItemList[UI_ITEM_LIST_EFX] = State->EfxItemList;
	State->IndexToItemList[UI_ITEM_LIST_EPS] = State->EpsItemList;
	State->IndexToItemList[UI_ITEM_LIST_MCL] = State->MclItemList;
	State->IndexToItemList[UI_ITEM_LIST_QD1] = State->Qd1ItemList;
	State->IndexToItemList[UI_ITEM_LIST_MD1] = State->Md1ItemList;
	State->IndexToItemList[UI_ITEM_LIST_MD2] = State->Md2ItemList;

	FilesList(Context->Config.Editor.ClientDataPath, "*.ebm", true, OnFilesList, State->EbmItemList);
	FilesList(Context->Config.Editor.ClientDataPath, "*.efx", true, OnFilesList, State->EfxItemList);
	FilesList(Context->Config.Editor.ClientDataPath, "*.eps", true, OnFilesList, State->EpsItemList);
	FilesList(Context->Config.Editor.ClientDataPath, "*.mcl", true, OnFilesList, State->MclItemList);

	Context->ArchiveItemType = -1;
	Context->Archive = NULL;
}

Void ControlUIDeinit(
	EditorContextRef Context,
	ControlUIStateRef State
) {
	UnloadArchive(Context, State);
	UIListDestroy(State->EbmItemList);
	UIListDestroy(State->EfxItemList);
	UIListDestroy(State->EpsItemList);
	UIListDestroy(State->MclItemList);
	UIListDestroy(State->Qd1ItemList);
	UIListDestroy(State->Md1ItemList);
	UIListDestroy(State->Md2ItemList);
}

Void ControlUIDraw(
	EditorContextRef Context,
	ControlUIStateRef State
) {
    Rectangle PanelFrame = { 0, 0, 288, 72 };
    GuiPanel(PanelFrame, NULL);

    Rectangle ToolbarFrame = { 8, 8, 24, 24 };
	if (!Context->Archive) {
		GuiToggleGroup(ToolbarFrame, State->ToolbarText, &State->ToolbarActiveIndex);
	}
	else {
		if (GuiButton(ToolbarFrame, "esc")) {
			UnloadArchive(Context, State);
		}
	}

	Rectangle SearchBoxFrame = { 8, 40, 272, 24 };
	if (State->ToolbarActiveIndex >= 0 && !Context->Archive) {
		UIListRef List = State->IndexToItemList[State->ToolbarActiveIndex];
		Rectangle ListFrame = { 0, 72, 288, Context->Config.Screen.Height - 72 };
		UIListItemRef Selection = NULL;
		if (UIListDraw(Context, List, ListFrame, &Selection)) {
			CString FilePath = (CString)UIListItemGetData(Selection);
			LoadArchive(Context, State, FilePath, State->ToolbarActiveIndex);

			if (State->ToolbarActiveIndex == UI_ITEM_LIST_MCL) {
				Context->Camera.position = (Vector3){ 0.0f, 128.0f, 0.0f };
				Context->Camera.target = (Vector3){ 128.0f, 0.0f, 128.0f };
				Context->Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
				Context->Camera.fovy = 45.0f;
				Context->Camera.projection = CAMERA_PERSPECTIVE;
			}
		}
	}
	
	/*
	if (State->ToolbarActiveIndex == 0) {		
		Int32 SearchQueryTextLength = strlen(State->SearchBoxText);
		if (GuiTextBox(SearchBoxFrame, State->SearchBoxText, UI_TEXT_LENGTH, State->SearchBoxEditMode)) {
			State->SearchBoxEditMode = !State->SearchBoxEditMode;
		}

		if (SearchQueryTextLength != strlen(State->SearchBoxText)) {
			ControlUIUpdateDungeonListItems(Context, State, State->SearchBoxText);
		}

		Int32 PreviousDungeonListActive = State->DungeonListActive;
		Rectangle DungeonListFrame = { 0, 72, 288, Context->Config.Screen.Height - 72 };
		GuiListView(DungeonListFrame, State->DungeonListItems, &State->DungeonListIndex, &State->DungeonListActive);
		if (PreviousDungeonListActive != State->DungeonListActive) {
			if (ArrayGetElementCount(Context->MissionDungeons) > State->DungeonListActive) {
				MissionDungeonDataRef Data = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons, State->DungeonListTargets[State->DungeonListActive]);

				Int DungeonIndex = Data->QDungeonIdx;
				State->DungeonListActiveName = DictionaryLookup(Context->MissionDungeonNames, &DungeonIndex);
				
				CString FileName = DictionaryLookup(Context->MissionDungeonFiles, &DungeonIndex);
				if (FileName) {
					CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Map", FileName, NULL);
					Trace(FilePath);

					Context->Terrain = MCLArchiveCreate(Context->Allocator);
					if (!MCLArchiveLoadFromFile(Context, Context->Terrain, FilePath)) {
						MCLArchiveDestroy(Context->Terrain);
						Context->Terrain = NULL;
					}

					State->ToolbarActiveIndex = 1;
				}
			}
		}
	}
	else if (State->ToolbarActiveIndex == 1) {
		if (State->DungeonListActiveName) GuiLabel(SearchBoxFrame, State->DungeonListActiveName);
	}
	else if (State->ToolbarActiveIndex == 2) {

	}*/
}
