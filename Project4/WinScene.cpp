/**
* Author: Belinda Weng
* Assignment: Rise of the AI
* Date due: 2025-04-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "WinScene.h"
#include "Utility.h"

WinScene::WinScene() {
    // Initialize member variables
    win_message = "YOU WIN!";
    restart_prompt = "PRESS ENTER TO PLAY AGAIN";
}

void WinScene::initialise() {
    // Load the font texture
    font_texture_id = Utility::load_texture("assets/font1.png");

    // Initialize game state
    m_game_state.next_scene_id = -1;
}

void WinScene::update(float delta_time) {
    // Nothing to update in the win scene
}

void WinScene::render(ShaderProgram* program) {
    // Set the viewport for the menu (centered)
    glm::mat4 view_matrix = glm::mat4(1.0f);
    program->set_view_matrix(view_matrix);

    // Render the win message (larger text, positioned at the top center)
    Utility::draw_text(program, font_texture_id, win_message, 0.4f, 0.2f, glm::vec3(-4.0f, 2.0f, 0.0f));

    // Render the restart prompt (positioned below the message)
    Utility::draw_text(program, font_texture_id, restart_prompt, 0.25f, 0.05f, glm::vec3(-4.0f, 0.0f, 0.0f));
}
