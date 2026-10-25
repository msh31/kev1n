#include "camera.hpp"
#include "globals.hpp"
#include "robot.hpp"

#include <fstream>

#include <emscripten.h>

double toast_expire_time = 0.00, rec_save_toast = 0.00;
double accumulator = 0.00;
constexpr double step_val = 0.05;
int hit_count = 0;
unsigned int random_seed = 69;

size_t replay_index = 0;
json replay_data{};

CCamera camera;
auto world = std::make_unique<CWorld>();
auto robot = std::make_unique<CRobot>(20, 30, *world); // bit odd

enum class State { MENU, PLAYING };
State state = State::MENU;

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

void game() {
    // Updates
    camera.update();

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

    // Drawing
    BeginDrawing();
    ClearBackground(BLACK);

    if (toast_expire_time > GetTime()) {
        DrawText("Target reached!", 100, 100, 20, PINK);
    }

    BeginMode2D(camera.get());
    world->render();
    robot->draw(ORANGE);
    EndMode2D();

    DrawText(TextFormat("Hit count: %d", hit_count), 100, 50, 20, GREEN);

    //#ifndef NDEBUG
    //        if ( IsKeyDown( KEY_W ) ) robot->move( Direction::UP );
    //        if ( IsKeyDown( KEY_S ) ) robot->move( Direction::DOWN );
    //        if ( IsKeyDown( KEY_A ) ) robot->move( Direction::LEFT );
    //        if ( IsKeyDown( KEY_D ) ) robot->move( Direction::RIGHT );
    //#endif // !NDEBUG

    EndDrawing();
}

void setup_game(unsigned int seed) {
    world->reset();
    robot = std::make_unique<CRobot>(20, 30, *world);
    replay_index = 0;
    accumulator = 0;
    hit_count = 0;
    toast_expire_time = 0;

    SetRandomSeed(seed);

    // Items
    int placed_obstacles = 0;
    while (placed_obstacles < 169) {
        int x = GetRandomValue(0, g_world_size - 1);
        int y = GetRandomValue(0, g_world_size - 1);
        if (world->get_cell_type(x, y) != ItemType::NONE) continue;
        if (x == robot->get_pos().first && y == robot->get_pos().second) continue;

        world->spawn_item(x, y, 40, ItemType::OBSTACLE);
        placed_obstacles += 1;
    }

    // Target
    world->spawn_item(33, 42, 40, ItemType::TARGET);
}

bool start_run(int index) {
    std::filesystem::path p = std::format("rec{}.json", index);

    std::ifstream f(p);
    if (!f.is_open()) {
        std::println(stderr, "[-] failed to open recording '{}'!", p.string());
        return false;
    }

    try {
        replay_data = json::parse(f);

        auto seed = replay_data.at("seed").get<unsigned int>();
        setup_game(seed);
    }
    catch (json::exception& ex) {
        std::println("[-] failed to parse recording '{}'!", p.string());
        return false;
    }

    return true;
}

std::array<Rectangle, 3> buttonz{};
void menu() {
    BeginDrawing();
    ClearBackground(BLACK);

    auto sh_c = GetScreenHeight() / 2;
    auto sw_c = GetScreenWidth() / 2 - MeasureText("Kev1n Demo", 50) / 2;

    DrawText("Kev1n Demo", sw_c, sh_c, 50, GREEN);

    for (size_t i = 0; i < buttonz.size(); ++i) {
        buttonz[i] = Rectangle{ (float)(GetScreenWidth() / 2 - 100), (float)(sh_c + 80 + i * 70), 200, 50 };
        DrawRectangleRec(buttonz[i], GRAY);

        auto str = TextFormat("Run %zu", i + 1);
        auto str_len = MeasureText(str, 20);
        DrawText(str, buttonz[i].x + (buttonz[i].width - str_len) / 2, buttonz[i].y + (buttonz[i].height - 20) / 2, 20, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), buttonz[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            //std::println("clicked {}", i);
            if (start_run(i + 1)) state = State::PLAYING;
        }
    }

    EndDrawing();
}

void frame() {
    switch (state) {
    case State::MENU:
        menu();
        break;
    case State::PLAYING:
        game();
        break;
    default:
        menu();
    }
}

auto main( ) -> int {
    const char* window_name = "Kev1n";
#ifndef NDEBUG
    window_name = "Kev1n [DEBUG]";
#endif

    InitWindow( g_window_width, g_window_height, window_name );
    SetTargetFPS( 60 ); //no delta time?????? - yes

    //setup_game(random_seed);

    // Main lop
    emscripten_set_main_loop(frame, 0, 1);

    CloseWindow( );
    return 0;
}
