//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#include "Entity.hpp"

float framesPerSecond = 30.0f;

Entity::Entity () {
    scale = glm::vec3(0.5f, 0.5f, 0.0f);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    friction = glm::vec3(1.2f, 1.2f, 0.0f);
    size = glm::vec3(0.5f, 0.5f, 0.0f);
    
    isStatic = true;
    isVisible = true;
    
    currentIndex = 0;
    animationElapsed = 0.0f;
    
    health = 3;
    numCoins = 0;
    hasKey = false;

    resetCollisions();
}

void Entity::Update(float elapsed){
}

void Entity::Render(ShaderProgram &program){
    spriteSheet.DrawSpriteSheetSprite(program, currentIndex, spriteSheet.textureID);
}

void Entity::resetCollisions() {
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
}

void Entity::setVelocity(float elapsed){
    velocity.x = lerp(velocity.x, 0.0f, elapsed * friction.x);
    velocity.y = lerp(velocity.y, 0.0f, elapsed * friction.y);
    velocity.y += gravity.y * elapsed;
    velocity.x += acceleration.x * elapsed;
}
