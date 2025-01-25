#include "Context.h"
#include "ControlUI.h"

Void UnloadArchive(
	EditorContextRef Context,
	ControlUIStateRef State
) {
	if (!Context->CanvasView->Content) return;

	switch (Context->CanvasView->ContentType) {
	case UI_ITEM_LIST_EBM: {
		EBMArchiveDestroy((EBMArchiveRef)Context->CanvasView->Content);
		break;
	}

	case UI_ITEM_LIST_EFX: {
		break;
	}

	case UI_ITEM_LIST_EPS: {
		break;
	}

	case UI_ITEM_LIST_MCL: {
		MCLArchiveDestroy((MCLArchiveRef)Context->CanvasView->Content);
		break;
	}

	case UI_ITEM_LIST_QD1: {
		MCLArchiveDestroy((MCLArchiveRef)Context->CanvasView->Content);
		break;
	}

	case UI_ITEM_LIST_MD1: {
		MCLArchiveDestroy((MCLArchiveRef)Context->CanvasView->Content);
		break;
	}

	case UI_ITEM_LIST_MD2: {
		MCLArchiveDestroy((MCLArchiveRef)Context->CanvasView->Content);
		break;
	}

	case UI_ITEM_LIST_MOB: {
		EBMArchiveDestroy((EBMArchiveRef)Context->CanvasView->Content);
		break;
	}

	default:
		break;
	}

	UIViewSetContent(Context->CanvasView, -1, NULL);
}

Void LoadArchive(
	EditorContextRef Context,
	ControlUIStateRef State,
	CString FilePath,
	Int32 ArchiveItemType
) {
	UnloadArchive(Context, State);

	switch (ArchiveItemType) {
	case UI_ITEM_LIST_EBM:
	case UI_ITEM_LIST_MOB: {
		EBMArchiveRef Archive = EBMArchiveCreate(Context->Allocator);
		if (EBMArchiveLoadFromFile(Context->ShaderEBM, Archive, FilePath)) {
			UIViewSetContent(Context->CanvasView, UI_ITEM_LIST_EBM, Archive);
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
			
			Archive->AnimationIndex = -1;
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
			UIViewSetContent(Context->CanvasView, UI_ITEM_LIST_MCL, Archive);
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
	CStringCopySafe(State->ToolbarText, sizeof(State->ToolbarText), "ebm;efx;eps;mcl;qd1;md1;md2;mob");
	State->ToolbarActiveIndex = 0;

	for (Int Index = 0; Index < UI_ITEM_LIST_COUNT; Index += 1) {
		State->ItemLists[Index] = UIListCreate(Context->Allocator, PLATFORM_PATH_MAX);
	}

	FilesList(Context->Config.Editor.ClientDataPath, "*.ebm", true, OnFilesList, State->ItemLists[UI_ITEM_LIST_EBM]);
	FilesList(Context->Config.Editor.ClientDataPath, "*.efx", true, OnFilesList, State->ItemLists[UI_ITEM_LIST_EFX]);
	FilesList(Context->Config.Editor.ClientDataPath, "*.eps", true, OnFilesList, State->ItemLists[UI_ITEM_LIST_EPS]);
	FilesList(Context->Config.Editor.ClientDataPath, "*.mcl", true, OnFilesList, State->ItemLists[UI_ITEM_LIST_MCL]);

	for (Int32 Index = 0; Index < ArrayGetElementCount(Context->MissionDungeons1); Index += 1) {
		MissionDungeonDataRef DungeonData = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons1, Index);

		Int DungeonIndex = DungeonData->QDungeonIdx;
		CString Name = DictionaryLookup(Context->MissionDungeonNames, &DungeonIndex);
		if (Name) {
			UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MD1], Name, &Index, sizeof(Index));
		}
		else {
			Char TempName[64] = { 0 };
			sprintf(TempName, "dungeon_%d;", DungeonData->QDungeonIdx);
			UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MD1], Name, &Index, sizeof(Index));
		}
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Context->MissionDungeons2); Index += 1) {
		MissionDungeonDataRef DungeonData = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons2, Index);

		Int DungeonIndex = DungeonData->QDungeonIdx;
		CString Name = DictionaryLookup(Context->MissionDungeonNames, &DungeonIndex);
		if (Name) {
			UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MD2], Name, &Index, sizeof(Index));
		}
		else {
			Char TempName[64] = { 0 };
			sprintf(TempName, "dungeon_%d;", DungeonData->QDungeonIdx);
			UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MD2], Name, &Index, sizeof(Index));
		}
	}

	for (Int Index = 0; Index < Context->MobSpeciesCount; Index += 1) {
		CString Name = DictionaryLookup(Context->MobSpeciesNames, Context->MobSpeciesData[Index].TextName);
		if (Name) {
			UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MOB], Name, &Index, sizeof(Index));
		}
		else {
			if (strlen(Context->MobSpeciesData[Index].TextName) > 0) {
				UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MOB], Context->MobSpeciesData[Index].TextName, &Index, sizeof(Index));
			}
			else {
				UIListAppendItem(State->ItemLists[UI_ITEM_LIST_MOB], "<null>", &Index, sizeof(Index));
			}
		}
	}
}

Void ControlUIDeinit(
	EditorContextRef Context,
	ControlUIStateRef State
) {
	UnloadArchive(Context, State);
	
	for (Int Index = 0; Index < UI_ITEM_LIST_COUNT; Index += 1) {
		UIListDestroy(State->ItemLists[Index]);
	}
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
	if (!Context->CanvasView->Content) {
		GuiToggleGroup(ToolbarFrame, State->ToolbarText, &State->ToolbarActiveIndex);
	}
	else {
		if (GuiButton(ToolbarFrame, "esc")) {
			UnloadArchive(Context, State);
		}
	}

	Rectangle SearchBoxFrame = { 8, 40, 272, 24 };
	if (State->ToolbarActiveIndex >= 0 && !Context->CanvasView->Content) {
		UIListRef List = State->ItemLists[State->ToolbarActiveIndex];
		Rectangle ListFrame = { 0, 72, 288, Context->Config.Screen.Height - 72 };
		UIListItemRef Selection = NULL;

		Int32 SearchQueryTextLength = strlen(State->SearchBoxText);
		if (GuiTextBox(SearchBoxFrame, State->SearchBoxText, UI_TEXT_LENGTH, State->SearchBoxEditMode)) {
			State->SearchBoxEditMode = !State->SearchBoxEditMode;
		}

		if (SearchQueryTextLength != strlen(State->SearchBoxText)) {
			UIListFilter(List, State->SearchBoxText);
		}

		if (UIListDraw(Context, List, ListFrame, &Selection)) {
			if (State->ToolbarActiveIndex == UI_ITEM_LIST_QD1) {
			}
			else if (State->ToolbarActiveIndex == UI_ITEM_LIST_MD1) {
				Int32 Index = *(Int32*)UIListItemGetData(Selection);
				MissionDungeonDataRef Data = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons1, Index);
				Int DungeonIndex = Data->QDungeonIdx;
				CString FileName = DictionaryLookup(Context->MissionDungeonFiles, &DungeonIndex);
				if (FileName) {
					CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Map", FileName, NULL);
					LoadArchive(Context, State, FilePath, UI_ITEM_LIST_MCL);
				}
			}
			else if (State->ToolbarActiveIndex == UI_ITEM_LIST_MD2) {
				Int32 Index = *(Int32*)UIListItemGetData(Selection);
				MissionDungeonDataRef Data = (MissionDungeonDataRef)ArrayGetElementAtIndex(Context->MissionDungeons2, Index);
				Int DungeonIndex = Data->QDungeonIdx;
				CString FileName = DictionaryLookup(Context->MissionDungeonFiles, &DungeonIndex);
				if (FileName) {
					CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Map", FileName, NULL);
					LoadArchive(Context, State, FilePath, UI_ITEM_LIST_MCL);
				}
			}
			else if (State->ToolbarActiveIndex == UI_ITEM_LIST_MOB) {
				Int32 Index = *(Int32*)UIListItemGetData(Selection);
				MobSpeciesDataRef MobData = (MobSpeciesDataRef)&Context->MobSpeciesData[Index];
				CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, MobData->FileName, NULL);
				LoadArchive(Context, State, FilePath, UI_ITEM_LIST_MOB);
			}
			else {
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
	}

	/*
	if (Context->Archive && (Context->ArchiveItemType == UI_ITEM_LIST_EBM || Context->ArchiveItemType == UI_ITEM_LIST_MOB)) {
		EBMArchiveRef Archive = (EBMArchiveRef)Context->Archive;

		Rectangle ListFrame = { 0, 72, 288, Context->Config.Screen.Height - 72 };
		UIListItemRef Selection = NULL;

//		if (UIListDraw(Context, Context->ModelList, ListFrame, &Selection)) {
//			Context->ModelIndex = *(Int32*)UIListItemGetData(Selection);
//		}

		if (UIListDraw(Context, Context->AnimationList, ListFrame, &Selection)) {
			Int32 AnimationIndex = *(Int32*)UIListItemGetData(Selection);
			EBMArchiveStartAnimationAtIndex(Archive, AnimationIndex);
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
	*/



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
