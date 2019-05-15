//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <SDL.h>
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SpriteSheet.hpp"
using namespace std;

enum EntityType { ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_PLATFORM, ENTITY_ITEM};
enum ItemType { ITEM_COIN, ITEM_KEY, ITEM_DOOR, ITEM_NONE };

class Entity {
public:
    // Methods
    Entity();
    
    void Update(float elapsed);
    void Render(ShaderProgram &program);
    
    void resetCollisions();
    
    void setVelocity(float elapsed);
    
    //Attributes
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 scale;
    glm::vec3 gravity;
    glm::vec3 friction;
    
    bool isStatic;
    bool isVisible;
    
    SpriteSheet spriteSheet;
    
    EntityType entityType;
    ItemType itemType;
    
    int currentIndex;           //animation index
    float animationElapsed;
    
    vector<int> walkingIndices;
    
    int numCoins;
    int health;
    bool hasKey;
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedRight;
    bool collidedLeft;
};

#endif /* Entity_hpp */
