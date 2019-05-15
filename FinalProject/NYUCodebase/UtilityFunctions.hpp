//
//  UtilityFunctions.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/6/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef UtilityFunctions_hpp
#define UtilityFunctions_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

GLuint LoadTexture(const char *filePath);
void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing);
void DrawButton(ShaderProgram &program, int buttonTexture);
void DrawBackground(GLuint textureID, ShaderProgram& program);
float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax);
float lerp(float v0, float v1, float t);
float easeIn(float from, float to, float time);
float easeOut(float from, float to, float time);
void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

#endif /* UtilityFunctions_hpp */
