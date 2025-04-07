#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map* map;
    Entity* player;
    Entity* enemies;

    // ————— AUDIO ————— //
    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;

    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;

    // ————— PLAYER LIVES ————— //
    int lives;
    bool game_over;

    // ————— FONT TEXTURE ————— //
    GLuint font_texture_id;
};

class Scene {
protected:
    GameState m_game_state;

public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    // ————— LIVES MANAGEMENT ————— //
    void lose_life();
    void check_game_over();
    void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position);

    // ————— SETTERS ————— //
    void set_lives(int lives) { m_game_state.lives = lives; }
    void set_game_over(bool game_over) { m_game_state.game_over = game_over; }

    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state; }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
    int get_lives() const { return m_game_state.lives; }
    bool is_game_over() const { return m_game_state.game_over; }
};