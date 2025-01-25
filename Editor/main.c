#include "Context.h"
#include "ControlUI.h"
#include "DataLoader.h"
#include "EBMShader.h"
#include "MCLArchive.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui/raygui.h"
#undef RAYGUI_IMPLEMENTATION

Void LoadMobSpeciesData(
    EditorContextRef Context
) {
    CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Language", Context->Config.Editor.ClientLanguage, "cabal_msg.enc.xml", NULL);
    ArchiveRef Archive = ArchiveCreateEmpty(Context->Allocator);
//    if (!ArchiveLoadFromFileEncrypted(Archive, FilePath, true)) {
    if (!ArchiveLoadFromFile(Archive, FilePath, true)) {
        ArchiveDestroy(Archive);
        return;
    }

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, -1, "cabal_message.mob_msg.msg");
    while (Iterator) {
        Char ID[PLATFORM_PATH_MAX] = { 0 };
        Char Name[PLATFORM_PATH_MAX] = { 0 };
        if (!ParseAttributeString(Archive, Iterator->Index, "id", ID, PLATFORM_PATH_MAX)) return;
        if (!ParseAttributeString(Archive, Iterator->Index, "cont", Name, PLATFORM_PATH_MAX)) return;

        DictionaryInsert(Context->MobSpeciesNames, ID, Name, strlen(Name) + 1);
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ArchiveDestroy(Archive);

    FileRef MobSpeciesFile = FileOpen(PathCombineAll(Context->Config.Editor.ClientDataPath, "mob.dat", NULL));
    if (MobSpeciesFile) {
        Int32 ReadLength = 0;
        FileRead(MobSpeciesFile, (UInt8**)&Context->MobSpeciesData, &ReadLength);
        FileClose(MobSpeciesFile);
        Context->MobSpeciesCount = ReadLength / sizeof(struct _MobSpeciesData);
    }
}

Void InitEditorViews(
    EditorContextRef Context
) {
    Context->CanvasView = UIViewCreate(Context->Allocator, (Rectangle) { 288, 0, GetScreenWidth() - 288 * 2, GetScreenHeight() });
    UIViewSetRenderCallback(Context->CanvasView, UI_ITEM_LIST_EBM, EBMArchiveDraw);
    UIViewSetRenderCallback(Context->CanvasView, UI_ITEM_LIST_MCL, DrawTerrain);
    ArrayAppendElement(Context->Views3D, &Context->CanvasView);
}

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[PLATFORM_PATH_MAX] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, PLATFORM_PATH_MAX);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "Editor.ini");
    struct _EditorContext ContextMemory = { 0 };
    EditorContextRef Context = &ContextMemory;
    Context->Allocator = AllocatorGetDefault();
    Context->Config = EditorConfigLoad(ConfigFilePath);
    Context->State.State = EDITOR_STATE_INIT;
    Context->ModelList = NULL;
    Context->MissionDungeonNames = IndexDictionaryCreate(Context->Allocator, 1024);
    Context->MissionDungeonFiles = IndexDictionaryCreate(Context->Allocator, 1024);
    Context->QuestDungeons1 = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonData), 8);
    Context->MissionDungeons1 = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonData), 8);
    Context->MissionDungeons2 = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonData), 8);
    Context->PatternParts = ArrayCreateEmpty(Context->Allocator, sizeof(struct _PatternPartData), 8);
    Context->TimeControls = ArrayCreateEmpty(Context->Allocator, sizeof(struct _TimeControlData), 8);
    Context->ArenaDefences = ArrayCreateEmpty(Context->Allocator, sizeof(struct _ArenaDefenceData), 8);
    Context->DungeonPointRewards = ArrayCreateEmpty(Context->Allocator, sizeof(struct _DungeonPointRewardData), 8);
    Context->MobMaps = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MobMapData), 8);
    Context->Triggers = ArrayCreateEmpty(Context->Allocator, sizeof(struct _TriggerData), 8);
    Context->ActionGroups = ArrayCreateEmpty(Context->Allocator, sizeof(struct _ActionGroupData), 8);
    Context->MissionMobMaps = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionMobMapData), 8);
    Context->ClearConditions = ArrayCreateEmpty(Context->Allocator, sizeof(struct _ClearConditionData), 8);
    Context->StartKills = ArrayCreateEmpty(Context->Allocator, sizeof(struct _StartKillData), 8);
    Context->MissionDungeonTimers = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonTimerData), 8);
    Context->MissionDungeonFlowChecks = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonFlowCheckData), 8);
    Context->MissionDungeonTimerValues = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonTimerValueData), 8);
    Context->ClientImmunes = ArrayCreateEmpty(Context->Allocator, sizeof(struct _ClientImmuneData), 8);
    Context->MobSpeciesNames = CStringDictionaryCreate(Context->Allocator, 8192);
    Context->Views2D = ArrayCreateEmpty(Context->Allocator, sizeof(UIViewRef), 8);
    Context->Views3D = ArrayCreateEmpty(Context->Allocator, sizeof(UIViewRef), 8);

    DiagnosticSetupLogFile(argv[0], Context->Config.Diagnostic.LogLevel, NULL, NULL);

    InitWindow(Context->Config.Screen.Width, Context->Config.Screen.Height, TARGET_NAME);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(Context->Config.Screen.TargetFPS);

    LoadMissionDungeonLocales(Context, "cont_msg.enc");
    LoadMissionDungeonLocales(Context, "cont2_msg.enc");
    LoadMissionDungeonLocales(Context, "cont3_msg.enc");
    LoadMissionDungeonFiles(Context, "cont2.enc");
    LoadMissionDungeonFiles(Context, "cont3.enc");
    LoadMissionDungeonData(Context, Context->MissionDungeons1, "MissionDungeon.scp");
    LoadMissionDungeonData(Context, Context->MissionDungeons2, "MissionDungeon2.scp");
    LoadMobSpeciesData(Context);

    Context->Camera.position = (Vector3){ 0.0f, 128.0f, 0.0f };
    Context->Camera.target = (Vector3){ 128.0f, 0.0f, 128.0f };
    Context->Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Context->Camera.fovy = 45.0f;
    Context->Camera.projection = CAMERA_PERSPECTIVE;
    Context->ShaderEBM = EBMShaderLoad(Context);

    InitEditorViews(Context);
    ControlUIInit(Context, &Context->ControlState);

    rlEnableColorBlend();
    rlEnableDepthTest();
    rlEnableDepthMask();
    rlDisableBackfaceCulling();

    while (!WindowShouldClose()) {
        Vector2 MousePosition = GetMousePosition();
        Vector2 MouseDelta = GetMouseDelta();
        Vector3 Movement = { 0, 0, 0 };
        Vector3 Rotation = { 0, 0, 0 };
        Float32 Distance = Vector3Distance(Context->Camera.position, Context->Camera.target);
        Float32 Zoom = GetMouseWheelMove() * -powf(Distance, 1.5f) * GetFrameTime();

        if (MousePosition.x > 288) {
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector3 Target = Context->Camera.target;
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    if (Context->CanvasView->ContentType == UI_ITEM_LIST_MCL) {
                        Ray Ray = GetMouseRay(MousePosition, Context->Camera);
                        RayCollision Collision = MCLArchiveTraceRay((MCLArchiveRef)Context->CanvasView->Content, Ray);
                        if (Collision.hit) Target = Collision.point;
                    }
                }

                Bool RotateAroundTarget = true;
                CameraYaw(&Context->Camera, -MouseDelta.x * 8.0f * GetFrameTime(), RotateAroundTarget);
                CameraPitch(&Context->Camera, -MouseDelta.y * 8.0f * GetFrameTime(), true, RotateAroundTarget, false);
                Context->Camera.target = Target;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Bool InWorldPlane = true;
                CameraMoveForward(&Context->Camera, MouseDelta.y * 16.0f * GetFrameTime(), InWorldPlane);
                CameraMoveRight(&Context->Camera, -MouseDelta.x * 16.0f * GetFrameTime(), InWorldPlane);
            }

            UpdateCameraPro(&Context->Camera, Movement, Rotation, Zoom);
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(Context->Camera);

            for (Int Index = 0; Index < ArrayGetElementCount(Context->Views3D); Index += 1) {
                UIViewRef View = *(UIViewRef*)ArrayGetElementAtIndex(Context->Views3D, Index);
                UIViewDraw(Context, View);
            }

            /*
            {
                if (Context->Archive) {
                    if (Context->ArchiveItemType == UI_ITEM_LIST_EBM ||
                        Context->ArchiveItemType == UI_ITEM_LIST_MOB) {
                        EBMArchiveDraw(Context, (EBMArchiveRef)Context->Archive);
                    }

                    if (Context->ArchiveItemType == UI_ITEM_LIST_MCL) {
                        DrawTerrain(Context, (MCLArchiveRef)Context->Archive);
                    }

                } else {
                    DrawGrid(256, 1);
                }
            }*/
            EndMode3D();

            for (Int Index = 0; Index < ArrayGetElementCount(Context->Views2D); Index += 1) {
                UIViewRef View = *(UIViewRef*)ArrayGetElementAtIndex(Context->Views2D, Index);
                UIViewDraw(Context, View);
            }

            ControlUIDraw(Context, &Context->ControlState);
        }
        EndDrawing();
    }

    ControlUIDeinit(Context, &Context->ControlState);
    CloseWindow();
    DiagnosticTeardown();

    UIListDestroy(Context->ModelList);
    if (Context->MobSpeciesData) free(Context->MobSpeciesData);
    DictionaryDestroy(Context->MobSpeciesNames);

    for (Int Index = 0; Index < ArrayGetElementCount(Context->Views2D); Index += 1) {
        UIViewRef View = *(UIViewRef*)ArrayGetElementAtIndex(Context->Views2D, Index);
        UIViewDestroy(View);
    }

    ArrayDestroy(Context->Views2D);

    for (Int Index = 0; Index < ArrayGetElementCount(Context->Views3D); Index += 1) {
        UIViewRef View = *(UIViewRef*)ArrayGetElementAtIndex(Context->Views3D, Index);
        UIViewDestroy(View);
    }

    ArrayDestroy(Context->Views3D);

    return 0;
}