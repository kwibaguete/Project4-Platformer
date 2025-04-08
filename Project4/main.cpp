/**
* Author: Belinda Weng
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "MenuScene.h"
#include "LoseScene.h"
#include "WinScene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"

// ----- CONSTANTS ----- //
constexpr int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
FONT_FILEPATH[] = "assets/font1.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };
enum GameScenes { MENU, LEVEL_A, LEVEL_B, LEVEL_C, LOSE_SCREEN, WIN_SCREEN };

// ----- GLOBAL VARIABLES ----- //
Scene* g_current_scene;
MenuScene* g_menu_scene;
LoseScene* g_lose_scene;
WinScene* g_win_scene;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
GameScenes g_current_scene_id = MENU;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// Global lives counter
int g_lives = 3;
bool g_game_over = false;
GLuint g_font_texture_id;

void switch_to_scene(Scene* scene)
{
    // Save the current lives count before switching scenes
    int current_lives = 3;
    bool current_game_over = false;

    if (g_current_scene != nullptr) {
        current_lives = g_current_scene->get_lives();
        current_game_over = g_current_scene->is_game_over();
    }

    g_current_scene = scene;
    g_current_scene->initialise();

    // Transfer the lives count to the new scene
    g_current_scene->set_lives(current_lives);
    g_current_scene->set_game_over(current_game_over);
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ----- VIDEO ----- //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Project 4",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ----- GENERAL ----- //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // Initialize global lives
    g_lives = 3;
    g_game_over = false;

    // Load font texture
    g_font_texture_id = Utility::load_texture(FONT_FILEPATH);

    // ----- LEVEL SETUP ----- //
    g_menu_scene = new MenuScene();
    g_lose_scene = new LoseScene();
    g_win_scene = new WinScene();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();

    // Start with the menu scene
    switch_to_scene(g_menu_scene);

    // ----- BLENDING ----- //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ----- KEYSTROKES ----- //
        switch (event.type) {
            // ----- END GAME ----- //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;

            case SDLK_RETURN:
                // If we're in the menu scene, start the game
                if (g_current_scene_id == MENU) {
                    switch_to_scene(g_level_a);
                    g_current_scene_id = LEVEL_A;
                }
                // If we're in the lose screen, restart the game
                else if (g_current_scene_id == LOSE_SCREEN || g_current_scene_id == WIN_SCREEN) {
                    // Reset lives and game over state
                    g_lives = 3;
                    g_game_over = false;

                    // Switch back to level A
                    switch_to_scene(g_level_a);
                    g_current_scene_id = LEVEL_A;
                }
                break;

            case SDLK_SPACE:
                // ----- JUMPING ----- //
                if (g_current_scene_id != MENU && g_current_scene_id != LOSE_SCREEN &&
                    g_current_scene_id != WIN_SCREEN && g_current_scene->get_state().player->get_collided_bottom())
                {
                    g_current_scene->get_state().player->jump();
                    Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                }
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    // Only process movement input if we're in a gameplay level
    if (g_current_scene_id != MENU && g_current_scene_id != LOSE_SCREEN && g_current_scene_id != WIN_SCREEN) {
        // Don't process input if game is over
        if (g_current_scene->is_game_over()) {
            return;
        }

        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

        // ----- KEY HOLD ----- //
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    // ----- DELTA TIME / FIXED TIME STEP CALCULATION ----- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ----- UPDATING THE SCENE (i.e. map, character, enemies...) ----- //
        g_current_scene->update(FIXED_TIMESTEP);

        // Update global game state from current scene (only if in a gameplay level)
        if (g_current_scene_id != MENU && g_current_scene_id != LOSE_SCREEN && g_current_scene_id != WIN_SCREEN) {
            g_lives = g_current_scene->get_lives();
            g_game_over = g_current_scene->is_game_over();

            // Check if player has lost all lives and switch to lose screen
            if (g_game_over) {
                switch_to_scene(g_lose_scene);
                g_current_scene_id = LOSE_SCREEN;
            }

            // Check if we need to switch to win screen (special case for level C)
            if (g_current_scene_id == LEVEL_C && g_current_scene->get_state().next_scene_id == 3) {
                switch_to_scene(g_win_scene);
                g_current_scene_id = WIN_SCREEN;
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // ----- PLAYER CAMERA ----- //
    if (g_current_scene_id != MENU && g_current_scene_id != LOSE_SCREEN && g_current_scene_id != WIN_SCREEN) {
        g_view_matrix = glm::mat4(1.0f);

        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }

        // Check if we need to switch scenes
        int next_scene_id = g_current_scene->get_state().next_scene_id;
        if (next_scene_id != -1 && !g_game_over) {
            switch (next_scene_id) {
            case 1:  // Switch to LevelB
                switch_to_scene(g_level_b);
                g_current_scene_id = LEVEL_B;
                break;
            case 2:  // Switch to LevelC
                switch_to_scene(g_level_c);
                g_current_scene_id = LEVEL_C;
                break;
            default:
                break;
            }
        }
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ----- RENDERING THE SCENE (i.e. map, character, enemies...) ----- //
    g_current_scene->render(&g_shader_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ----- DELETING LEVEL DATA (i.e. map, character, enemies...) ----- //
    delete g_menu_scene;
    delete g_lose_scene;
    delete g_win_scene;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
}

// ----- GAME LOOP ----- //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
