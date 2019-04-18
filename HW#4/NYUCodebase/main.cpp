/*
 Goal: Collect all the diamonds
 
 Use left and right arrows to move and space bar to jump
 */

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// 60 FPS (1.0f/60.0f) (update sixty times a second)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

class Entity;

SDL_Window* displayWindow;
ShaderProgram program;
glm::mat4 projectionMatrix;
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
SDL_Event event;
bool done;
float lastFrameTicks;
float ticks;
float elapsed;
float accumulator;
GLuint textTexture;
GLuint spriteSheetTexture;
GLuint playerTexture;

vector<Entity> coins;

float friction_x = 1.2f;
float friction_y = 1.5f;

float gravity_x = 1.5f;
float gravity_y = -2.5f;

int spriteCountX = 8;
int spriteCountY = 4;

const int runAnimation[] = {8, 9, 10, 11, 12, 13, 14, 15};
const int numFrames = 8;
float animationElapsed = 0.0f;
float framesPerSecond = 30.0f;
int currentIndex = 0;

int mapWidth;
int mapHeight;

string levelFile;

#define LEVEL_HEIGHT 17
#define LEVEL_WIDTH 17
unsigned char ** levelData;
unsigned int ** solidData;

//Sprite ount of map sprite sheet
#define SPRITE_COUNT_X 30
#define SPRITE_COUNT_Y 30

#define TILE_SIZE 0.5f

const Uint8 *keys = SDL_GetKeyboardState(NULL);

void DrawSpriteSheetSprite(ShaderProgram &program, GLint texture, int index, int spriteCountX, int spriteCountY) {
    glBindTexture(GL_TEXTURE_2D, texture);
    
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

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN, ENTITY_DIAMOND};

class Entity {
public:
    int index;
    
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    bool visible;
    EntityType entityType;
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};

GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return retTexture;
}

void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing) {
    
    float character_size = 1.0/16.0f;
    
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for(int i=0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + character_size,
            texture_x + character_size, texture_y,
            texture_x + character_size, texture_y + character_size,
            texture_x + character_size, texture_y,
            texture_x, texture_y + character_size,
        });
    }
    
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void DrawTileMap(){
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for(int y=0; y < LEVEL_HEIGHT; y++) {
        for(int x=0; x < LEVEL_WIDTH; x++) {
            if(levelData[y][x] != 0) {
                float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
                float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
                
                float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
                float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
                
                vertexData.insert(vertexData.end(), {
                    TILE_SIZE * x, -TILE_SIZE * y,
                    TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                    TILE_SIZE * x, -TILE_SIZE * y,
                    (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
                });
                texCoordData.insert(texCoordData.end(), {
                    u, v,
                    u, v+(spriteHeight),
                    u+spriteWidth, v+(spriteHeight),
                    u, v,
                    u+spriteWidth, v+(spriteHeight),
                    u+spriteWidth, v
                });
            }
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)(vertexData.size()/2));
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void placeEntity(string type, float placeX, float placeY){
    Entity entity;
    
    cout << placeX << " " << placeY << endl;
    
    if(type == "Diamond"){
        entity.entityType = ENTITY_DIAMOND;
        entity.index = 379;
    }
    else if(type == "Coin"){
        entity.entityType = ENTITY_COIN;
        entity.index = 77;
    }
    
    entity.position = glm::vec3(placeX + TILE_SIZE/2, placeY + TILE_SIZE/2, 0.0f);
    entity.size.x = 0.5f;
    entity.size.y = 0.5f;
    entity.visible = true;
    coins.push_back(entity);
}

bool readHeader(std::ifstream &stream) {
    string line;
    mapWidth = -1;
    mapHeight = -1;
    
    while(getline(stream, line)) {
        if(line == "") {
            break;
        }
        
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        }
        else if(key == "height"){
            mapHeight = atoi(value.c_str());
        }
    }
    
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    }
    else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        solidData = new unsigned int*[mapHeight];
        
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
            solidData[i] = new unsigned int[mapWidth];
        }
        return true;
    }
}

bool readLayerData(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") {
            break;
        }
        
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if(key == "data") {
            for(int y=0; y < mapHeight; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                for(int x=0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    if(val > 0) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        levelData[y][x] = val-1;
                        solidData[y][x] = 1;
                    } else {
                        levelData[y][x] = 0;
                    }
                }
            }
        }
    }
    
    return true;
}

bool readEntityData(std::ifstream &stream) {
    string line;
    string type;
    while(getline(stream, line)) {
        if(line == "") {
            break;
        }
        
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if(key == "type") {
            type = value;
        }
        else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity(type, placeX, placeY);
        }
    }
    
    return true;
    
}

void openTextFile(string filename){
    ifstream infile(levelFile);
    string line;
    
    if( !infile  ) {
        cerr << "Can't open file " << levelFile << std::endl;
        exit( -1 );
    }
    
    while (getline(infile, line)) {
        
        if(line == "[header]") {
            if(!readHeader(infile)) {
                return;
            }
        } else if(line == "[layer]") {
            readLayerData(infile);
            
        } else if(line == "[ObjectsLayer]") {
            readEntityData(infile);
        }
    }
}

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}

Entity player;

bool checkTileCollision(int checkX, int checkY, int tileX, int tileY){
    //Check if tile coordinates are negative or larger than map
    if (checkX < 0) return false;
    if (checkY < 0) return false;
    if (checkX > mapWidth) return false;
    if (checkY > mapHeight) return false;
    
    bool collision = false;
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
    
    if (solidData[checkY][checkX] == 1){
        float posX = (checkX * TILE_SIZE);
        float posY = (-checkY * TILE_SIZE);
        
        if (player.position.y - player.size.y/2 <= posY){
            bottom = true;   //Bottom
        }
        if (player.position.y + player.size.y/2 >= posY - TILE_SIZE){
            top = true;      //Top
        }
        if (player.position.x + player.size.x/2 <= posX + TILE_SIZE){
            left = true;     //Left
        }
        if (player.position.x - player.size.x/2 >= posX){
            right = true;    //Right
        }
        
        collision = (top | bottom | left | right);
    }
    return collision;
}

void checkObjectCollision(){
    float distancex;
    float distancey;
    for(int j=0; j < coins.size(); j++) {
        distancex = abs(player.position.x - coins[j].position.x) - (coins[j].size.x + player.size.x)/2 + 0.15;
        distancey = abs(coins[j].position.y - player.position.y) - (coins[j].size.y + player.size.y)/2 + 0.15;
        
        if(distancex < 0 && distancey < 0){
            coins[j].visible = false;
        }
    }
}

bool collision;
int gridY;
int gridX;

void collisionY(){
    //Bottom
    worldToTileCoordinates(player.position.x, (player.position.y - player.size.y/2), &gridX, &gridY);
    collision = checkTileCollision(gridX, gridY, gridX, gridY);
    if(collision){
        player.velocity.y = 0;
        player.position.y += 0.0005 + (-(gridY) * TILE_SIZE) - (player.position.y - player.size.y/2);
        player.collidedBottom = true;
        collision = false;
    }
    
    //Top
    worldToTileCoordinates(player.position.x, (player.position.y + player.size.y/2 - 0.225), &gridX, &gridY);
    collision = checkTileCollision(gridX, gridY, gridX, gridY);
    if(collision){
        player.velocity.y = 0;
        player.position.y += -0.0005 + 0.225 + (player.position.y + player.size.y/2) - (-(gridY) * TILE_SIZE);
        player.collidedTop = true;
        collision = false;
    }
}

void collisionX(){
    //Left
    worldToTileCoordinates((player.position.x - player.size.x/2 + 0.2), player.position.y, &gridX, &gridY);
    collision = checkTileCollision(gridX, gridY, gridX, gridY);
    if(collision){
        player.velocity.x = 0;
        player.position.x += -0.0005 - 0.225 + (player.position.x - player.size.x/2) - ((gridX) * TILE_SIZE);
        player.collidedLeft = true;
        collision = false;
    }
    
    //Right
    worldToTileCoordinates((player.position.x + player.size.x/2 - 0.175), player.position.y, &gridX, &gridY);
    collision = checkTileCollision(gridX, gridY, gridX, gridY);
    if(collision){
        player.velocity.x = 0;
        player.position.x += -0.0005 + 0.225 + (player.position.x - player.size.x/2) - ((gridX) * TILE_SIZE);
        player.collidedRight = true;
        collision = false;
    }
}

void Setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    textTexture = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
    
    projectionMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    
    lastFrameTicks = 0.0f;
    elapsed = 0.0f;
    accumulator = 0.0f;
    
    player.position = glm::vec3(8.0f * TILE_SIZE, -14.0f * TILE_SIZE, 0.0f);

    player.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    player.size.x = 0.5f;
    player.size.y = 0.5f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");
    playerTexture = LoadTexture(RESOURCE_FOLDER"characters_3.png");
    
    levelFile = RESOURCE_FOLDER"Platformer2.txt";
    
    openTextFile(levelFile);
    
    glUseProgram(program.programID);
}

void ProcessInput(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
               player.acceleration.x = -15.0f;
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT){
                player.acceleration.x = 15.0f;
            }
        }
        else if (event.type == SDL_KEYUP){
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && player.collidedBottom == true){
                player.velocity.y = 3.5f;
            }
        }
    }
}

void Update(float elapsed){
    player.collidedTop = false;
    player.collidedBottom = false;
    player.collidedLeft = false;
    player.collidedRight = false;
    
    if(keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT]){
        animationElapsed += elapsed;
        if(animationElapsed > 1.0/framesPerSecond) {
            currentIndex++;
            animationElapsed = 0.0;
            if(currentIndex > numFrames-1) {
                currentIndex = 0;
            }
        }
    }
    else{
        currentIndex = 0;
    }
    
    player.velocity.x = lerp(player.velocity.x, 0.0f, elapsed * friction_x);
    player.velocity.y = lerp(player.velocity.y, 0.0f, elapsed * friction_y);

    player.velocity.y += gravity_y * elapsed;

    player.velocity.x += player.acceleration.x * elapsed;
    
    player.position.y += player.velocity.y * elapsed;
    collisionY();
    player.position.x += player.velocity.x * elapsed;
    collisionX();
    
    player.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);\
    
    checkObjectCollision();
}

void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    
    viewMatrix = glm::translate(modelMatrix, (glm::vec3(0.0f, 0.0f, 0.0f) - player.position));
    program.SetViewMatrix(viewMatrix);
    
    modelMatrix = glm::translate(modelMatrix, player.position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawSpriteSheetSprite(program, playerTexture, runAnimation[currentIndex], 8, 4);
    
    int counter = 0;    //count number of objects not visible
    
    for(int i = 0; i < coins.size(); i++){
        if(coins[i].visible == true){
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, coins[i].position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            
            DrawSpriteSheetSprite(program, spriteSheetTexture, coins[i].index, SPRITE_COUNT_X, SPRITE_COUNT_Y);
        }
        else{
            counter++;
            cout << counter << endl;
        }
    }
    
    modelMatrix = glm::mat4(1.0f);
    program.SetModelMatrix(modelMatrix);
    
    DrawTileMap();
    
    if(counter == coins.size()){
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, player.position - glm::vec3(0.95f, 0.0f, 0.0f));
        program.SetModelMatrix(modelMatrix);
        
        DrawText(program, textTexture, "Game Over", 0.145, 0.1);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Cleanup(){
    SDL_Quit();
}


int main(int argc, char *argv[])
{
    Setup();
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    done = false;
    
    while (!done) {
        ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if(elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue;
        }
        while(elapsed >= FIXED_TIMESTEP) {
            Update(FIXED_TIMESTEP);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        ProcessInput();
        Render();
    }
    
    Cleanup();
    return 0;
}
