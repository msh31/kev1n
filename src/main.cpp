#include "camera.hpp"
#include "world.hpp"

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

    //World
    auto world = std::make_unique<CWorld>();

    InitWindow(g_window_width, g_window_height, "Kevigator");
    SetTargetFPS(60);

    world->spawn(580, 77, 30, ItemType::OBSTACLE);

    while (!WindowShouldClose()) {
        //Updates
        camera.update();

        //Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera.get());
        world->render();
        EndMode2D();

        DrawCircleV(GetMousePosition(), 4, DARKGRAY);
        DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
            Vector2Add(GetMousePosition(), { -44, -24 }), 20, 2, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
