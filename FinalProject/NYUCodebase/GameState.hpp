//
//  GameState.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/7/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "ShaderProgram.h"
#include "UtilityFunctions.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SpriteSheet.hpp"
#include "Entity.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum Level {LEVEL1, LEVEL2, LEVEL3};

class GameState {
public:
    void SetUp(GLuint spriteTexture, GLuint textTexture, GLuint buttonTexture, GLuint playerTexture, GLuint playerFlippedTexture, GLuint backgroundTexture);
    void ResetLevel();
    
    void ProcessInput(SDL_Event& event);
    void Render(ShaderProgram &program);
    void Update(float elapsed);
    
    bool getGameOver();
    bool getGameWin();
    
    void ResetPlayer();
    
    //Drawing Level Maps
    void DrawTileMap(ShaderProgram& program);
    
    void placeEntity(string type, float placeX, float placeY);
    
    bool readHeader(std::ifstream &stream);
    bool readLayerData(std::ifstream &stream);
    bool readEntityData(std::ifstream &stream);
    void openTextFile(string filename);
    
    //Collisions
    bool checkTileCollision(Entity &entity, int checkX, int checkY, int tileX, int tileY);
    void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
    void collisionX(Entity &entity);
    void collisionY(Entity &entity);
    void BeforeCollisionX(Entity &entity);
    void checkObjectCollision();
    
    //Collisions
    bool collision;
    int gridY;
    int gridX;
    
    //Tile Map
    int LEVEL_HEIGHT;
    int LEVEL_WIDTH;
    float TILE_SIZE;
    
    //Sprite count of map sprite sheet
    int SPRITE_COUNT_X;
    int SPRITE_COUNT_Y;

    //Sound
    Mix_Chunk *coinCollect;
    Mix_Chunk *hitEnemy;
    
private:
    Level level;
    
    string levelFile;
    
    int mapWidth;
    int mapHeight;
    
    unsigned int ** levelData;
    unsigned int ** solidData;
    
    bool reset;
    bool quitGame;
    bool gameover;
    bool gameWin;
    bool fallDown;
    
    float timer; //Used in Level 3
    
    Entity player;
    
    int lives;
    int randomIndex; //Used in Level 3
    float xVelocity;  //Used in Level 3
    float animationTime;
    float yPos;
    
    SpriteSheet instructionsLabel;
    SpriteSheet coinCountLabel;
    SpriteSheet quitButton;
    
    vector<Entity> items;
    vector<Entity> enemies;
    vector<Entity> playerHearts;
    
    SpriteSheet playerSprites;
    SpriteSheet playerFlippedSprites;
    SpriteSheet levelSprites;
    GLuint selectPlayerOne;
    
    GLuint textTexture;
    GLuint buttonTexture;
    GLuint spriteTexture;
    GLuint playerTexture;
    GLuint playerFlippedTexture;
    GLuint backgroundTexture; //For Game Over
};

#endif /* GameState_hpp */
