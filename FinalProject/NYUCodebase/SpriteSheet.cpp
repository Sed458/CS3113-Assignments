//
//  SpriteSheet.cpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet() {}

SpriteSheet::SpriteSheet(int countX, int countY, GLuint texture) {
    spriteCountX = countX;
    spriteCountY = countY;
    spriteWidth = 1.0 / (float) spriteCountX;
    spriteHeight = 1.0 / (float) spriteCountY;
    textureID = texture;
}

SpriteSheet::SpriteSheet(unsigned int textureID, float u, float v, float width, float height, float size){
    this->textureID = textureID;
    this->u = u;
    this->v = v;
    this->spriteWidth = width;
    this->spriteHeight = height;
    this->size = size;
}

void SpriteSheet::DrawSpriteSheetSprite(ShaderProgram &program, int index, GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    float u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
    float v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;
    float spriteWidth = 1.0/(float)spriteCountX;
    float spriteHeight = 1.0/(float)spriteCountY;
    float texCoords[] = {
        u, v+spriteHeight,
        u+spriteWidth, v,
        u, v,
        u+spriteWidth, v,
        u, v+spriteHeight,
        u+spriteWidth, v+spriteHeight
    };
    float vertices[] = {-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  -0.5f,
        -0.5f, 0.5f, -0.5f};
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void SpriteSheet::Draw(ShaderProgram &program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    GLfloat texCoords[] = {
        u, v + spriteHeight,
        u + spriteWidth, v,
        u, v,
        u + spriteWidth, v,
        u, v + spriteHeight,
        u + spriteWidth, v + spriteHeight
    };
    
    float aspect = spriteWidth / spriteHeight;
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, -0.5f * size,
    };
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}
