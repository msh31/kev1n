#include "camera.hpp"

auto main() -> int {
    //Window
    constexpr int g_window_width = 1600;
    constexpr int g_window_height = 900;

    //Text
    const int g_text_x = 190;
    const int g_text_y = 200;
    int g_font_size = 20;

    //2D camera
    CCamera camera;

    InitWindow(g_window_width, g_window_height, "Kevigator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        //Update
        camera.update();

        //Drawing


        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera.get());
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
        if (camera.get_zoom_mode() == 0) DrawText("Mouse left button drag to move, mouse wheel to zoom", 20, 50, 20, DARKGRAY);
        else DrawText("Mouse left button drag to move, mouse press and move to zoom", 20, 50, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
