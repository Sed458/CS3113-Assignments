//
//  StartScreen.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/6/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef StartScreen_hpp
#define StartScreen_hpp

#include <stdio.h>
#include <SDL.h>
#include "ShaderProgram.h"
#include "UtilityFunctions.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SpriteSheet.hpp"

class StartScreen{
public:
    void Setup(GLuint tTexture, GLuint bTexture);
    void ProcessInput(SDL_Event event);
    void Render(ShaderProgram& program);
    void Update(float elapsed);
    
    bool GameStart();
    
    GLuint buttonTexture;
    GLuint textTexture;
    
    SpriteSheet startButton;
    
    bool start;
    bool fallDown;
    
    float yPos;
    
};

#endif /* StartScreen_hpp */
