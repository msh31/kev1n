#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

auto main() -> int {
    //Window
    constexpr int g_window_width = 1600;
    constexpr int g_window_height = 900;

    //Text
    const int g_text_x = 190;
    const int g_text_y = 200;
    int g_font_size = 20;

    //2D camera
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    int zoomMode = 0;       // 0-Mouse Wheel, 1-Mouse Move



    InitWindow(g_window_width, g_window_height, "Kevigator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        //Update
        if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        if (zoomMode == 0) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;

                float scale = 0.2f * wheel;
                camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            }
        }
        else {
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                float deltaX = GetMouseDelta().x;
                float scale = 0.005f * deltaX;
                camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            }
        }

        //Drawing


        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
        rlPushMatrix();
        rlTranslatef(0, 25 * 50, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(100, 50);
        rlPopMatrix();

        DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 50, MAROON);
        EndMode2D();

        DrawCircleV(GetMousePosition(), 4, DARKGRAY);
        DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
            Vector2Add(GetMousePosition(), { -44, -24 }), 20, 2, BLACK);

        DrawText("[1][2] Select mouse zoom mode (Wheel or Move)", 20, 20, 20, DARKGRAY);
        if (zoomMode == 0) DrawText("Mouse left button drag to move, mouse wheel to zoom", 20, 50, 20, DARKGRAY);
        else DrawText("Mouse left button drag to move, mouse press and move to zoom", 20, 50, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
