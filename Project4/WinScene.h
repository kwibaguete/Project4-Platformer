#pragma once
#include "Scene.h"

class WinScene : public Scene {
public:
    // ————— CONSTRUCTORS ————— //
    WinScene();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

private:
    // ————— ATTRIBUTES ————— //
    GLuint font_texture_id;
    std::string win_message;
    std::string restart_prompt;
};
