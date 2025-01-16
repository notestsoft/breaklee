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
		if (EBMArchiveLoadFromFile(Context->ShaderEBM, Archive, FilePath)) {
			Context->ArchiveItemType = ArchiveItemType;
			Context->Archive = Archive;
			EBMArchiveSetupCamera(Context, Archive);

			if (Context->ModelList) UIListDestroy(Context->ModelList);
			Context->ModelList = UIListCreate(Context->Allocator, sizeof(Int32));
			Context->ModelIndex = -1;

			for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
				EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
				UIListAppendItem(Context->ModelList, Mesh->Name, (UInt8*)&Index, sizeof(Int32));
			}

			if (Context->AnimationList) UIListDestroy(Context->AnimationList);
			Context->AnimationList = UIListCreate(Context->Allocator, sizeof(Int32));
			Context->AnimationIndex = -1;

			for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Animations); Index += 1) {
				EBMAnimationRef Animation = (EBMAnimationRef)ArrayGetElementAtIndex(Archive->Animations, Index);
				UIListAppendItem(Context->AnimationList, Animation->Name, (UInt8*)&Index, sizeof(Int32));
			}
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

float GetLuminance(Color color) {
	float r = (color.r / 255.0f);
	float g = (color.g / 255.0f);
	float b = (color.b / 255.0f);

	// Apply luminance correction factors based on the RGB components
	r = (r <= 0.03928f) ? r / 12.92f : powf((r + 0.055f) / 1.055f, 2.4f);
	g = (g <= 0.03928f) ? g / 12.92f : powf((g + 0.055f) / 1.055f, 2.4f);
	b = (b <= 0.03928f) ? b / 12.92f : powf((b + 0.055f) / 1.055f, 2.4f);

	return (0.2126f * r + 0.7152f * g + 0.0722f * b);
}

// Function to calculate the contrast ratio between two colors
float GetContrastRatio(Color color1, Color color2) {
	float lum1 = GetLuminance(color1) + 0.05f;
	float lum2 = GetLuminance(color2) + 0.05f;
	return (lum1 > lum2) ? (lum1 / lum2) : (lum2 / lum1);
}

// Function to choose the best text color (black or white) based on background luminance
Color GetAccessibleTextColor(Color background) {
	if (background.a < 1) return BLACK;
	Color textColor = (GetLuminance(background) > 0.5f) ? BLACK : WHITE;
	return textColor;
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

	if (Context->Archive && Context->ArchiveItemType == UI_ITEM_LIST_EBM) {
		EBMArchiveRef Archive = (EBMArchiveRef)Context->Archive;

		Rectangle ListFrame = { 0, 72, 288, Context->Config.Screen.Height - 72 };
		UIListItemRef Selection = NULL;

//		if (UIListDraw(Context, Context->ModelList, ListFrame, &Selection)) {
//			Context->ModelIndex = *(Int32*)UIListItemGetData(Selection);
//		}

		if (UIListDraw(Context, Context->AnimationList, ListFrame, &Selection)) {
			Context->AnimationIndex = *(Int32*)UIListItemGetData(Selection);
			Archive->ElapsedTime = 0.0f;
		}

		Char Value[128] = { 0 };
		Rectangle ValueFrame = { Context->Config.Screen.Width - 288, 0, 288, 20 };

		static Vector2 Scroll = { 0, 0 };
		Rectangle PanelFrame = { Context->Config.Screen.Width - 288, 0, 288, Context->Config.Screen.Height };
		Rectangle ContentFrame = { Context->Config.Screen.Width - 288, 0, 288, Context->Config.Screen.Height * ArrayGetElementCount(Archive->Materials) };
		Rectangle ViewFrame = { 0, 0, 0, 0 };
		GuiScrollPanel(PanelFrame, "", ContentFrame, &Scroll, &ViewFrame);

		ValueFrame.y += Scroll.y;
		Int MaterialIndex = -1;
		if (Context->ModelIndex >= 0) {
			EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Context->ModelIndex);
			MaterialIndex = Mesh->MaterialIndex;
		}

		for (Int Index = 0; Index < ArrayGetElementCount(Archive->Materials); Index += 1) {
			EBMMaterialRef Material = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, Index);
			if (MaterialIndex >= 0 && MaterialIndex != Index) continue;

			struct { CString Name; Vector4 Color; } Colors[] = {
				{ "Ambient", Material->Properties.Ambient },
				{ "Diffuse", Material->Properties.Diffuse },
				{ "Specular", Material->Properties.Specular },
				{ "Emission", Material->Properties.Emission },
			};

			for (Int Index = 0; Index < sizeof(Colors) / sizeof(Colors[0]); Index += 1) {
				Color Value = {
					Colors[Index].Color.x * 255,
					Colors[Index].Color.y * 255,
					Colors[Index].Color.z * 255,
					Colors[Index].Color.w * 255,
				};

				DrawRectangle(ValueFrame.x, ValueFrame.y, ValueFrame.width, ValueFrame.height, Value);
				DrawRectangleLinesEx(ValueFrame, 0.5f, RAYWHITE);
				DrawText(Colors[Index].Name, ValueFrame.x, ValueFrame.y + 4, 11, GetAccessibleTextColor(Value));
				ValueFrame.y += ValueFrame.height;
			}

			sprintf(Value, "Strength (%.4f)", Material->Properties.Strength);
			GuiLabel(ValueFrame, Value);
			ValueFrame.y += ValueFrame.height;

			GuiLabel(ValueFrame, "TextureMain");
			ValueFrame.y += ValueFrame.height;

			sprintf(Value, "IsFaceted (%d)", Material->TextureMain.IsFaceted);
			GuiLabel(ValueFrame, Value);
			ValueFrame.y += ValueFrame.height;

			sprintf(Value, "ScrollSpeed (%.4f, %.4f)", Material->TextureMain.ScrollSpeed.x, Material->TextureMain.ScrollSpeed.y);
			GuiLabel(ValueFrame, Value);
			ValueFrame.y += ValueFrame.height;

			ValueFrame.height = ValueFrame.width;
			DrawTexturePro(Material->TextureMain.Texture.Texture, (Rectangle) { 0, 0, Material->TextureMain.Texture.Image.width, Material->TextureMain.Texture.Image.height }, ValueFrame, (Vector2) { 0, 0 }, 0, WHITE);
			ValueFrame.y += ValueFrame.height;
			ValueFrame.height = 20;
	
			if (Material->TextureBlend.MaterialIndex >= 0) {
				EBMMaterialRef BlendMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, Material->TextureBlend.MaterialIndex);

				GuiLabel(ValueFrame, "TextureBlend");
				ValueFrame.y += ValueFrame.height;

				sprintf(Value, "IsFaceted (%d)", Material->TextureBlend.IsFaceted);
				GuiLabel(ValueFrame, Value);
				ValueFrame.y += ValueFrame.height;

				sprintf(Value, "ScrollSpeed (%.4f, %.4f)", Material->TextureBlend.ScrollSpeed.x, Material->TextureBlend.ScrollSpeed.y);
				GuiLabel(ValueFrame, Value);
				ValueFrame.y += ValueFrame.height;

				sprintf(Value, "BlendFlags (%d)", Material->TextureBlend.BlendFlags);
				GuiLabel(ValueFrame, Value);
				ValueFrame.y += ValueFrame.height;

				ValueFrame.height = ValueFrame.width;
				DrawTexturePro(BlendMaterial->TextureMain.Texture.Texture, (Rectangle) { 0, 0, BlendMaterial->TextureMain.Texture.Image.width, BlendMaterial->TextureMain.Texture.Image.height }, ValueFrame, (Vector2) { 0, 0 }, 0, WHITE);
				ValueFrame.y += ValueFrame.width;
				ValueFrame.y += ValueFrame.height;
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
