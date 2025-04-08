/**
* Author: Belinda Weng
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
#define LEVEL_END_X 12.0f  // Position where the level ends

constexpr char PLAYER_FILEPATH[] = "assets/pudding.png",
PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
ENEMY_FILEPATH[] = "assets/usagi.png",
FONT_FILEPATH[] = "assets/font1.png";

// Rename to LEVEL_A_DATA to avoid conflict with LevelB
unsigned int LEVEL_A_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelA::~LevelA()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.hurt_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_DATA, map_texture_id, 1.0f, 4, 1);

    GLuint player_texture_id = Utility::load_texture(PLAYER_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    // For a single sprite (pudding.png), set animation parameters to 0
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        0.0f,                      // animation time
        0,                         // animation frame amount (set to 0 for single sprite)
        0,                         // current animation index
        0,                         // animation column amount (set to 0 for single sprite)
        0,                         // animation row amount (set to 0 for single sprite)
        1.0f,                      // width
        1.0f,                      // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/yep.wav");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(60.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/sploosh.wav");
    m_game_state.hurt_sfx = Mix_LoadWAV("assets/damage.wav");

    // Set next scene ID to -1 (no transition yet)
    m_game_state.next_scene_id = -1;

    // Initialize lives and game over state
    m_game_state.lives = 3;
    m_game_state.game_over = false;

    // Load font texture
    m_game_state.font_texture_id = Utility::load_texture(FONT_FILEPATH);
}

void LevelA::update(float delta_time)
{
    if (m_game_state.game_over) {
        return;  // Don't update if game is over
    }

    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);

        // Check for collision with enemy
        if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
            lose_life();
            // Reset player position after collision
            m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
        }
    }

    // Check if player fell off the map (y position is too low)
    if (m_game_state.player->get_position().y < -10.0f) {
        lose_life();
        // Reset player position after falling
        m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    }

    // Check if player has reached the end of the level
    if (m_game_state.player->get_position().x >= LEVEL_END_X && !m_game_state.game_over) {
        // Set a flag to indicate level completion
        m_game_state.next_scene_id = 1;  // 1 will represent LevelB
    }
}


void LevelA::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
        m_game_state.enemies[i].render(g_shader_program);

    // Display lives
    std::string lives_text = "LIVES: " + std::to_string(m_game_state.lives);
    draw_text(g_shader_program, m_game_state.font_texture_id, lives_text, 0.5f, 0.5f, glm::vec3(-4.5f, 3.5f, 0.0f));

    // Display game over message if needed
    if (m_game_state.game_over) {
        GLuint g_font_texture_id = Utility::load_texture("assets/font1.png");
        draw_text(g_shader_program, g_font_texture_id, "YOU LOSE", 1.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}
