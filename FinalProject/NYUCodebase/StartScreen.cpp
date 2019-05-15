//
//  StartScreen.cpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/6/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#include "StartScreen.hpp"

float startButtonPos[] = { -0.84f, 0.84f, -0.31f, -0.71f };

void StartScreen::Setup(GLuint tTexture, GLuint bTexture){
    textTexture = tTexture;
    buttonTexture = bTexture;
    start = false;
    yPos = 0.0f;
    fallDown = true;
    
    SpriteSheet sSprite(buttonTexture, 0.0f/512.0f, 45.0f/256.0f, 190.0f/512.0f, 49.0f/256.0f, 0.2f);
    startButton = sSprite;
}

void StartScreen::ProcessInput(SDL_Event event){
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        float mouseX = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.x : event.motion.x;
        float mouseY = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.y : event.motion.y;
        
        float unitX = (((float)mouseX / 640.0f) * 3.554f ) - 1.777f;
        float unitY = (((float)(360-mouseY) / 360.0f) * 2.0f ) - 1.0f;
        
        if (unitX >= startButtonPos[0] && unitX <= startButtonPos[1] && unitY <= startButtonPos[2] && unitY >= startButtonPos[3]) {
            start = true;
        }
    }
}

float PosY = 3.0f;
void StartScreen::Render(ShaderProgram &program){
    //Draw Title
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.0f, yPos, 0.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, "Uncharted", 0.145, 0.1);
    
    //Draw Start Button
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 2.0f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    startButton.Draw(program);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.44f, -0.5f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, "Let's Play!", 0.25, 0.1);
}

float animationTime = 0.0f;
void StartScreen::Update(float elapsed){
    //Title Animation (Fall Down)
    animationTime = animationTime + elapsed;
    float animationValue = mapValue(animationTime, 0.0f, 1.0f, 0.0f, 1.0f);
    
    yPos = easeIn(2.0f, 0.0f, animationValue);
    
    if (fallDown) {
        fallDown = false;
        animationTime = 0.0f;
    }
}

bool StartScreen::GameStart(){
    return start;
}
