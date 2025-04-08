#include "MenuScene.h"
#include "Utility.h"

MenuScene::MenuScene() {
    // Initialize member variables
    game_title = "RISE OF THE AI";
    start_prompt = "PRESS ENTER TO START";
}

void MenuScene::initialise() {
    // Load the font texture
    font_texture_id = Utility::load_texture("assets/font1.png");

    // Initialize game state
    m_game_state.next_scene_id = -1;
}

void MenuScene::update(float delta_time) {
    // Nothing to update in the menu scene
}

void MenuScene::render(ShaderProgram* program) {
    // Set the viewport for the menu (centered)
    glm::mat4 view_matrix = glm::mat4(1.0f);
    program->set_view_matrix(view_matrix);

    // Render the game title (larger text, positioned at the top center)
    Utility::draw_text(program, font_texture_id, game_title, 0.4f, 0.2f, glm::vec3(-4.0f, 2.0f, 0.0f));

    // Render the start prompt (positioned below the title)
    Utility::draw_text(program, font_texture_id, start_prompt, 0.25f, 0.15f, glm::vec3(-4.0f, 0.0f, 0.0f));
}
