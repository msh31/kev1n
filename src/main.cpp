#include "globals.hpp"
#include "camera.hpp"
#include "robot.hpp"

auto main() -> int {
    //2D camera
    CCamera camera;

    //World
    auto world = std::make_unique<CWorld>();

    //Robot
    auto robot = std::make_unique<CRobot>(20, 30, *world); //bit odd

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
        robot->draw(ORANGE);
        EndMode2D();

        DrawCircleV(GetMousePosition(), 4, DARKGRAY);
        DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
            Vector2Add(GetMousePosition(), { -44, -24 }), 20, 2, BLACK);

#ifndef NDEBUG
        if (IsKeyDown(KEY_W)) robot->move(Direction::UP);
        if (IsKeyDown(KEY_S)) robot->move(Direction::DOWN);
        if (IsKeyDown(KEY_A)) robot->move(Direction::LEFT);
        if (IsKeyDown(KEY_D)) robot->move(Direction::RIGHT);
#endif // !NDEBUG

        EndDrawing();
    }

    CloseWindow();
    return 0;

}
