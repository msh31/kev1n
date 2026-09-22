#include "globals.hpp"
#include "camera.hpp"
#include "world.hpp"

auto main() -> int {
    //2D camera
    CCamera camera;

    //World
    auto world = std::make_unique<CWorld>();

    InitWindow(g_window_width, g_window_height, "Kev1n");
    SetTargetFPS(60);

    world->spawn_item(0, 0, 40, ItemType::OBSTACLE);

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
