#include "camera.hpp"
#include "globals.hpp"
#include "robot.hpp"

#include <fstream>
#include <filesystem>

double toast_expire_time = 0.00, rec_save_toast = 0.00;
double accumulator = 0.00;
constexpr double step_val = 0.05;
int hit_count = 0;
unsigned int random_seed = 69;

size_t replay_index = 0;
json replay_data{};

void step_replay(CRobot& r, CWorld& w) {
    if (replay_index >= replay_data["moves"].size()) return;
    if (accumulator < step_val) return;

    r.move(lookup.at(replay_data["moves"][replay_index].get<std::string>()));

    accumulator -= step_val;
    replay_index += 1;

    int robot_x = r.get_pos().first;
    int robot_y = r.get_pos().second;

    if (w.get_cell_type(robot_x, robot_y) == ItemType::TARGET) {
        r.reached_destination = true;
    }
}

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

    //probably logging to nothing here, TODO...
    if (argc > 1) {
        try {
            random_seed = std::stoul(argv[1]);
            std::string file_name = std::format("recording-{}.json", random_seed);
            if(!std::filesystem::exists(file_name)) {
                std::println("[Error] recording for '{}' does not exist!", random_seed);
                CloseWindow();
                return 1;
            }
            std::ifstream in(file_name);
            if (!in.is_open()) {
                std::println("[Error] failed to open recording for reading!");
                CloseWindow();
                return 1;
            };

            replay_data = json::parse(in);
        }
        catch (const json::exception& jex) {
            std::println("[Error] failed to parse recording file!");
            CloseWindow();
            return 1;
        }
        catch (const std::exception& ex) {
            std::println("[Error] input seed failed to convert, it may be non numeric or out of range");
            CloseWindow();
            return 1;
        }

        if (!replay_data.contains("moves")) {
            std::println("[Error] recording does not contain any moves!");
            CloseWindow();
            return 1;
        }
    }
    else {
        random_seed = GetRandomValue(0, 69420);
    }
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

    // Main lop
    while ( !WindowShouldClose( ) ) {
        // Updates
        camera.update( );

        accumulator += GetFrameTime();

        if (!replay_data.empty()) {
            step_replay(*robot, *world);
        }

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
        if (IsKeyPressed(KEY_R) && replay_data.empty()) {
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
            rec_save_toast = GetTime() + 3.0;
        }

        // Drawing
        BeginDrawing( );
        ClearBackground( BLACK );

        if (toast_expire_time > GetTime()) {
            DrawText("Target reached!", 100, 100, 20, PINK);
        }
        if (rec_save_toast > GetTime()) {
            auto txt = std::format("Saved recording here: {}", GetWorkingDirectory());
            DrawText(txt.c_str(), 100, 250, 20, BLUE);
        }

        BeginMode2D( camera.get( ) );
        world->render( );
        robot->draw( ORANGE );
        EndMode2D( );

        DrawText(TextFormat("Hit count: %d", hit_count), 100, 50, 20, GREEN);

//#ifndef NDEBUG
//        if ( IsKeyDown( KEY_W ) ) robot->move( Direction::UP );
//        if ( IsKeyDown( KEY_S ) ) robot->move( Direction::DOWN );
//        if ( IsKeyDown( KEY_A ) ) robot->move( Direction::LEFT );
//        if ( IsKeyDown( KEY_D ) ) robot->move( Direction::RIGHT );
//#endif // !NDEBUG

        EndDrawing( );
    }

    CloseWindow( );
    return 0;
}
