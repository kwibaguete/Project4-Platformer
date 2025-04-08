#pragma once
#include "Scene.h"

class LoseScene : public Scene {
public:
    // ————— CONSTRUCTORS ————— //
    LoseScene();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

private:
    // ————— ATTRIBUTES ————— //
    GLuint font_texture_id;
    std::string lose_message;
    std::string restart_prompt;
};
