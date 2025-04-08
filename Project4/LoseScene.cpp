/**
* Author: Belinda Weng
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LoseScene.h"
#include "Utility.h"

LoseScene::LoseScene() {
    // Initialize member variables
    lose_message = "YOU LOSE";
    restart_prompt = "PRESS Q TO EXIT";
}

void LoseScene::initialise() {
    // Load the font texture
    font_texture_id = Utility::load_texture("assets/font1.png");

    // Initialize game state
    m_game_state.next_scene_id = -1;
}

void LoseScene::update(float delta_time) {
    // Nothing to update in the lose scene
}

void LoseScene::render(ShaderProgram* program) {
    // Set the viewport for the menu (centered)
    glm::mat4 view_matrix = glm::mat4(1.0f);
    program->set_view_matrix(view_matrix);

    // Render the lose message (larger text, positioned at the top center)
    Utility::draw_text(program, font_texture_id, lose_message, 0.4f, 0.2f, glm::vec3(-4.0f, 2.0f, 0.0f));

    // Render the restart prompt (positioned below the message)
    Utility::draw_text(program, font_texture_id, restart_prompt, 0.25f, 0.15f, glm::vec3(-4.0f, 0.0f, 0.0f));
}
