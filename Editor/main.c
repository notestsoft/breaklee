#include "Context.h"
#include "ControlUI.h"
#include "DataLoader.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui/raygui.h"
#undef RAYGUI_IMPLEMENTATION

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[PLATFORM_PATH_MAX] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, PLATFORM_PATH_MAX);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "Editor.ini");
    struct _EditorContext ContextMemory = { 0 };
    EditorContextRef Context = &ContextMemory;
    Context->Config = EditorConfigLoad(ConfigFilePath);
    Context->State.State = EDITOR_STATE_INIT;
    Context->ArchiveItemType = -1;
    Context->Archive = NULL;
    Context->MissionDungeonNames = IndexDictionaryCreate(Context->Allocator, 1024);
    Context->MissionDungeonFiles = IndexDictionaryCreate(Context->Allocator, 1024);
    Context->MissionDungeons = ArrayCreateEmpty(Context->Allocator, sizeof(struct _MissionDungeonData), 8);
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

    DiagnosticSetupLogFile(argv[0], Context->Config.Diagnostic.LogLevel, NULL, NULL);

    InitWindow(Context->Config.Screen.Width, Context->Config.Screen.Height, TARGET_NAME);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(Context->Config.Screen.TargetFPS);

    LoadMissionDungeonLocales(Context, "cont_msg.enc");
    LoadMissionDungeonLocales(Context, "cont2_msg.enc");
    LoadMissionDungeonLocales(Context, "cont3_msg.enc");
    LoadMissionDungeonFiles(Context, "cont2.enc");
    LoadMissionDungeonFiles(Context, "cont3.enc");
    LoadMissionDungeonData(Context, "MissionDungeon.scp");
    LoadMissionDungeonData(Context, "MissionDungeon2.scp");

    Context->Camera.position = (Vector3){ 0.0f, 128.0f, 0.0f };
    Context->Camera.target = (Vector3){ 128.0f, 0.0f, 128.0f };
    Context->Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Context->Camera.fovy = 45.0f;
    Context->Camera.projection = CAMERA_PERSPECTIVE;

    ControlUIInit(Context, &Context->ControlState);

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
                Bool RotateAroundTarget = true;
                CameraYaw(&Context->Camera, -MouseDelta.x * 8.0f * GetFrameTime(), RotateAroundTarget);
                CameraPitch(&Context->Camera, -MouseDelta.y * 8.0f * GetFrameTime(), true, RotateAroundTarget, false);
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
            {
                if (Context->Archive) {
                    if (Context->ArchiveItemType == UI_ITEM_LIST_EBM) {
                        EBMArchiveDraw(Context, (EBMArchiveRef)Context->Archive);
                    }

                    if (Context->ArchiveItemType == UI_ITEM_LIST_MCL) {
                        DrawTerrain(Context, (MCLArchiveRef)Context->Archive);
                    }

                } else {
                    DrawGrid(256, 1);
                }
            }
            EndMode3D();

            ControlUIDraw(Context, &Context->ControlState);
        }
        EndDrawing();
    }

    ControlUIDeinit(Context, &Context->ControlState);
    CloseWindow();
    DiagnosticTeardown();

    return 0;
}