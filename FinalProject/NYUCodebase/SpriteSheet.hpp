//
//  SpriteSheet.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef SpriteSheet_hpp
#define SpriteSheet_hpp

#include <stdio.h>
#include <SDL.h>
#include "ShaderProgram.h"
#include <vector>
#include <iostream>
#include "UtilityFunctions.hpp"

class SpriteSheet {
public:
    SpriteSheet();
    SpriteSheet(int countX, int countY, GLuint texture);
    SpriteSheet(unsigned int textureID, float u, float v, float width, float height, float size);
    
    //Uniform Sprites
    void DrawSpriteSheetSprite (ShaderProgram &program, int index, GLuint textureID);
    
    //Non-Uniform Sprites
    void Draw(ShaderProgram &program);
    
    GLuint textureID;
    
private:
    float spriteWidth;
    float spriteHeight;
    
    //non-uniform sheets
    float u;
    float v;
    float size;
    float sheetWidth;
    float sheetHeight;
    
    //uniform sheets
    int spriteCountX;
    int spriteCountY;
};

#endif /* SpriteSheet_hpp */
