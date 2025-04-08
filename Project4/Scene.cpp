/**
* Author: Belinda Weng
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Scene.h"

void Scene::lose_life() {
    m_game_state.lives--;

    // Play damage sound
    Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);

    // Check if player has run out of lives
    if (m_game_state.lives <= 0) {
        m_game_state.lives = 0;  // Ensure lives don't go negative
        m_game_state.game_over = true;  // Set game over flag
    }
}

void Scene::check_game_over() {
    if (m_game_state.lives <= 0) {
        m_game_state.game_over = true;
    }
}

void Scene::draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position) {
    // Scale the size of the font
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    // Start at the bottom left of the screen
    float x_pos = position.x;
    float y_pos = position.y;

    // Draw each character
    for (int i = 0; i < text.size(); i++) {
        // Get the current character
        int character = (int)text[i];

        // Calculate the UV coordinates
        float u_coord = (float)(character % 16) / 16.0f;
        float v_coord = (float)(character / 16) / 16.0f;

        // Calculate the vertices
        float vertices[] = {
            x_pos, y_pos, position.z,
            x_pos, y_pos - height * screen_size, position.z,
            x_pos + width * screen_size, y_pos - height * screen_size, position.z,
            x_pos, y_pos, position.z,
            x_pos + width * screen_size, y_pos - height * screen_size, position.z,
            x_pos + width * screen_size, y_pos, position.z
        };

        // Calculate the texture coordinates
        float texture_coordinates[] = {
            u_coord, v_coord,
            u_coord, v_coord + height,
            u_coord + width, v_coord + height,
            u_coord, v_coord,
            u_coord + width, v_coord + height,
            u_coord + width, v_coord
        };

        // Render the character
        glBindTexture(GL_TEXTURE_2D, font_texture_id);
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 0, vertices);
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Move to the next character
        x_pos += spacing * screen_size;
    }
}