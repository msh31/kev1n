#include "camera.hpp"
#include "globals.hpp"
#include "robot.hpp"

#include <fstream>

double toast_expire_time = 0.00;
int hit_count = 0;
unsigned int random_seed = 69;

auto main( int argc, char** argv ) -> int {
    CCamera camera;
    auto world = std::make_unique<CWorld>( );
    auto robot = std::make_unique<CRobot>( 20, 30, *world ); // bit odd

    const char* window_name = "Kev1n";
#ifndef NDEBUG
    window_name = "Kev1n [DEBUG]";
#endif

    InitWindow( g_window_width, g_window_height, window_name );
    SetTargetFPS( 60 ); //no delta time?????? - yes

    random_seed = GetRandomValue(0, 69420);
    SetRandomSeed(random_seed);

    // Item spawns
    int placed_obstacles = 0;
    while (placed_obstacles < 169) {
        int x = GetRandomValue(0, g_world_size - 1);
        int y = GetRandomValue(0, g_world_size - 1);
        if (world->get_cell_type(x, y) != ItemType::NONE) continue;
        if (x == robot->get_pos().first && y == robot->get_pos().second) continue;

        world->spawn_item(x, y, 40, ItemType::OBSTACLE);
        placed_obstacles += 1;
    }
    world->spawn_item(33, 42, 40, ItemType::TARGET);

    while ( !WindowShouldClose( ) ) {
        // Updates
        camera.update( );
        robot->update();

        if (robot->reached_destination) {
            robot->reached_destination = false;
            toast_expire_time = GetTime() + 3.0;
            hit_count += 1;

            world->spawn_item(robot->get_pos().first, robot->get_pos().second, 40, ItemType::NONE); //overwrites old target - this sucks

            int robot_x = robot->get_pos().first;
            int robot_y = robot->get_pos().second;

            int new_x = robot_x;
            int new_y = robot_y;

            do {
                new_x = GetRandomValue(0, g_world_size - 1);
                new_y = GetRandomValue(0, g_world_size - 1);
            } while (world->get_cell_type(new_x, new_y) != ItemType::NONE || (new_x == robot_x && new_y == robot_y));

            world->spawn_item(new_x, new_y, 40, ItemType::TARGET);
        }

        // save recording
        if (IsKeyPressed(KEY_R)) {
            json j;
            std::string file_name = std::format("recording-{}.json", random_seed);
            std::ofstream out(file_name);
            if (!out.is_open()) continue; //should probably log this someday

            j["seed"] = random_seed;
            j["moves"] = json::array();

            for (const auto& r : robot->recordings()) {
                auto& it = reverse_lookup.at(r);
                j["moves"].push_back(it);
            }
            out << j.dump(4);
            out.close();
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

        DrawText(TextFormat("Hit count: %d", hit_count), 100, 50, 20, GREEN);

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
