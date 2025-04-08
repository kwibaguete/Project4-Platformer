#pragma once
#pragma once
#include "Scene.h"

class MenuScene : public Scene {
public:
    // ————— CONSTRUCTORS ————— //
    MenuScene();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

private:
    // ————— ATTRIBUTES ————— //
    GLuint font_texture_id;
    std::string game_title;
    std::string start_prompt;
};
