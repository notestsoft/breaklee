#include "Context.h"

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[PLATFORM_PATH_MAX] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, PLATFORM_PATH_MAX);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "Editor.ini");
    struct _EditorContext ContextMemory = { 0 };
    EditorContextRef Context = &ContextMemory;
    Context->Config = EditorConfigLoad(ConfigFilePath);
    Context->State.State = EDITOR_STATE_INIT;

    DiagnosticSetupLogFile(argv[0], Context->Config.Diagnostic.LogLevel, NULL, NULL);

    InitWindow(Context->Config.Screen.Width, Context->Config.Screen.Height, TARGET_NAME);
    SetTargetFPS(Context->Config.Screen.TargetFPS);

    Context->Camera.position = (Vector3){ 18.0f, 21.0f, 18.0f };
    Context->Camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    Context->Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Context->Camera.fovy = 45.0f;
    Context->Camera.projection = CAMERA_PERSPECTIVE;

    Image image = LoadImage("resources/heightmap.png");
    Texture2D texture = LoadTextureFromImage(image);
    Mesh mesh = GenMeshHeightmap(image, (Vector3) { 16, 8, 16 });
    Model model = LoadModelFromMesh(mesh);

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Vector3 mapPosition = { -8.0f, 0.0f, -8.0f };

    UnloadImage(image);

    while (!WindowShouldClose()) {
        UpdateCamera(&Context->Camera, CAMERA_ORBITAL);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(Context->Camera);
            {
                DrawModel(model, mapPosition, 1.0f, RED);

                DrawGrid(20, 1.0f);
            }
            EndMode3D();

            DrawTexture(texture, Context->Config.Screen.Width - texture.width - 20, 20, WHITE);
            DrawRectangleLines(Context->Config.Screen.Width - texture.width - 20, 20, texture.width, texture.height, GREEN);

            DrawFPS(10, 10);

        }
        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadModel(model);

    CloseWindow();
    DiagnosticTeardown();

    return 0;
}