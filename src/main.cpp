#include "camera.hpp"
#include "globals.hpp"
#include "robot.hpp"

double toast_expire_time = 0.00;

auto main( ) -> int {
    // 2D camera
    CCamera camera;

    // World
    auto world = std::make_unique<CWorld>( );

    // Robot
    auto robot = std::make_unique<CRobot>( 20, 30, *world ); // bit odd

    InitWindow( g_window_width, g_window_height, "Kev1n" );
    SetTargetFPS( 60 ); //no delta time?????? - yes

    //TODO: improve this
    world->spawn_item( 0, 0, 40, ItemType::OBSTACLE );
    world->spawn_item( 26, 30, 40, ItemType::TARGET );

    while ( !WindowShouldClose( ) ) {
        // Updates
        camera.update( );
        robot->update();

        if (robot->reached_destination) {
            robot->reached_destination = false;
            toast_expire_time = GetTime() + 3.0;
        }

        // Drawing
        BeginDrawing( );
        ClearBackground( BLACK );

        if (toast_expire_time > GetTime()) {
            DrawText("Target reached!", 100, 100, 20, PINK);
        }

        BeginMode2D( camera.get( ) );
        world->render( );
        robot->draw( ORANGE );
        EndMode2D( );

        DrawCircleV( GetMousePosition( ), 4, LIGHTGRAY);
        DrawTextEx(
            GetFontDefault( ), TextFormat( "[%i, %i]", GetMouseX( ), GetMouseY( ) ),
            Vector2Add( GetMousePosition( ), { -44, -24 } ), 20, 2, LIGHTGRAY );

#ifndef NDEBUG
        if ( IsKeyDown( KEY_W ) ) robot->move( Direction::UP );
        if ( IsKeyDown( KEY_S ) ) robot->move( Direction::DOWN );
        if ( IsKeyDown( KEY_A ) ) robot->move( Direction::LEFT );
        if ( IsKeyDown( KEY_D ) ) robot->move( Direction::RIGHT );
#endif // !NDEBUG

        EndDrawing( );
    }

    CloseWindow( );
    return 0;
}
