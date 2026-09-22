#include <raylib.h>

auto main() -> int {
    constexpr int g_window_width = 800;
    constexpr int g_window_height = 450;

    const int g_text_x = 190;
    const int g_text_y = 200;

    int g_font_size = 20;

    InitWindow(g_window_width, g_window_height, "Kevigator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);


        DrawText("hello world", g_text_x, g_text_y, g_font_size, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
