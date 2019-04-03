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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define MAX_BULLETS 30

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
float timer;
int bulletIndex;
int enemiesBulletIndex;
GLuint textTexture;
GLuint spriteSheetTexture;

const Uint8 *keys = SDL_GetKeyboardState(NULL);

class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
    void Draw(ShaderProgram &program);
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
};

SheetSprite::SheetSprite(){}

SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size){
    this->textureID = textureID;
    this->u = u;
    this->v = v;
    this->width = width;
    this->height = height;
    this->size = size;
}

void SheetSprite::Draw(ShaderProgram &program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };
    float aspect = width / height;
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size ,
        0.5f * size * aspect, -0.5f * size};
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

class Entity {
public:
    void Draw();
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 size;
    
    float rotation;
    
    SheetSprite sprite;
    
    bool dynamic;
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

struct GameState {
    Entity player;
    Entity enemies[24];
    Entity bullets[MAX_BULLETS];
    Entity enemiesBullets[10];
    int score;
};

GameState state;

void CreateGameState(){
    state.score = 0;
    
    state.player.sprite = SheetSprite(spriteSheetTexture, 224.0f/1024.0f, 832.0f/1024.0f, 99.0f/1024.0f, 75.0f/1024.0f, 0.2f);
    state.player.position = glm::vec3(0.0f, -0.75f, 0.0f);
    state.player.dynamic = true;
    
    float enemyPositiony = 0.65f;
    float enemyPositionx = -1.0f;
    
    int enemiesIndex = 0;
    
    for(int i=0; i < 6; i++) {
        Entity myEntity;
        myEntity.sprite = SheetSprite(spriteSheetTexture, 444.0f/1024.0f, 0.0f/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 0.2);
        myEntity.position.x = enemyPositionx;
        myEntity.position.y = enemyPositiony;
        myEntity.velocity.x = 0.2f;
        myEntity.dynamic = true;
        state.enemies[enemiesIndex] = myEntity;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = 0.4f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        Entity myEntity;
        myEntity.sprite = SheetSprite(spriteSheetTexture, 518.0f/1024.0f, 493.0f/1024.0f, 82.0f/1024.0f, 84.0f/1024.0f, 0.2);
        myEntity.position.x = enemyPositionx;
        myEntity.position.y = enemyPositiony;
        myEntity.velocity.x = 0.2f;
        myEntity.dynamic = true;
        state.enemies[enemiesIndex] = myEntity;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = 0.15f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        Entity myEntity;
        myEntity.sprite = SheetSprite(spriteSheetTexture, 444.0f/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 0.2);
        myEntity.position.x = enemyPositionx;
        myEntity.position.y = enemyPositiony;
        myEntity.velocity.x = 0.2f;
        myEntity.dynamic = true;
        state.enemies[enemiesIndex] = myEntity;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = -0.1f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        Entity myEntity;
        myEntity.sprite = SheetSprite(spriteSheetTexture, 423.0f/1024.0f, 728.0f/1024.0f, 93.0f/1024.0f, 84.0f/1024.0f, 0.2);
        myEntity.position.x = enemyPositionx;
        myEntity.position.y = enemyPositiony;
        myEntity.velocity.x = 0.2f;
        myEntity.dynamic = true;
        state.enemies[enemiesIndex] = myEntity;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    for(int i=0; i < MAX_BULLETS; i++) {
        state.bullets[i].sprite = SheetSprite(spriteSheetTexture, 856.0f/1024.0f, 421.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.2f);
        state.bullets[i].position.x = -2000.0f;
    }
    
    for(int i=0; i < 10; i++) {
        state.enemiesBullets[i].sprite = SheetSprite(spriteSheetTexture, 856.0f/1024.0f, 812.0f/1024.0f, 9.0f/1024.0f, 57.0f/1024.0f, 0.2f);
        state.enemiesBullets[i].position.x = -2000.0f;
    }
    
    bulletIndex = 0;
}

void ResetGameState(){
    state.score = 0;
    
    state.player.position = glm::vec3(0.0f, -0.75f, 0.0f);
    state.player.dynamic = true;
    
    float enemyPositiony = 0.65f;
    float enemyPositionx = -1.0f;
    
    int enemiesIndex = 0;
    
    for(int i=0; i < 6; i++) {
        state.enemies[enemiesIndex].position.x = enemyPositionx;
        state.enemies[enemiesIndex].position.y = enemyPositiony;
        state.enemies[enemiesIndex].dynamic = true;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = 0.4f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        state.enemies[enemiesIndex].position.x = enemyPositionx;
        state.enemies[enemiesIndex].position.y = enemyPositiony;
        state.enemies[enemiesIndex].dynamic = true;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = 0.15f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        state.enemies[enemiesIndex].position.x = enemyPositionx;
        state.enemies[enemiesIndex].position.y = enemyPositiony;
        state.enemies[enemiesIndex].dynamic = true;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    enemyPositiony = -0.1f;
    enemyPositionx = -1.0f;
    
    for(int i=0; i < 6; i++) {
        state.enemies[enemiesIndex].position.x = enemyPositionx;
        state.enemies[enemiesIndex].position.y = enemyPositiony;
        state.enemies[enemiesIndex].dynamic = true;
        
        enemiesIndex += 1;
        enemyPositionx += 0.3;
    }
    
    for(int i=0; i < MAX_BULLETS; i++) {
        state.bullets[i].position.x = -2000.0f;
    }
    
    for(int i=0; i < 10; i++) {
        state.enemiesBullets[i].position.x = -2000.0f;
    }
    
    bulletIndex = 0;
    enemiesBulletIndex = 0;
}

enum GameMode { STATE_START_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER};

GameMode mode;

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
    timer = 0.0f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    mode = STATE_START_SCREEN;
    
    spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"sheet.png");
    
    CreateGameState();
    
    glUseProgram(program.programID);
}

void shootBullet() {
    state.bullets[bulletIndex].position.x = state.player.position.x;
    state.bullets[bulletIndex].position.y = state.player.position.y;
    
    state.bullets[bulletIndex].velocity.y = 2.5f;
    
    bulletIndex++;
    if(bulletIndex > MAX_BULLETS-1) {
        bulletIndex = 0;
    }
}

void enemyShootBullet(int index) {
    state.enemiesBullets[enemiesBulletIndex].position.x = state.enemies[index].position.x;
    state.enemiesBullets[enemiesBulletIndex].position.y = state.enemies[index].position.y;
    
    state.enemiesBullets[enemiesBulletIndex].velocity.y = -1.0f;
    
    enemiesBulletIndex++;
    if(enemiesBulletIndex > 9) {
        enemiesBulletIndex = 0;
    }
}

void ProcessStartScreenInput(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                mode = STATE_GAME_LEVEL;
            }
        }
    }
}

void ProcessGameLevelInput(GameState &state){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if(keys[SDL_SCANCODE_LEFT] && (state.player.position.x - FIXED_TIMESTEP * 5.0f) > -1.7) {
                state.player.position.x -= FIXED_TIMESTEP * 5.0f;
            }
            else if(keys[SDL_SCANCODE_RIGHT] && (state.player.position.x + FIXED_TIMESTEP * 5.0f) < 1.7) {
                state.player.position.x += FIXED_TIMESTEP * 5.0f;
            }
            if(event.key.keysym.scancode == SDL_SCANCODE_Q){
                mode = STATE_GAME_OVER;
            }
        }
        else if (event.type == SDL_KEYUP) {
            if(event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                shootBullet();
            }
        }
    }
}

void ProcessGameOverInput(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if(event.key.keysym.scancode == SDL_SCANCODE_M){
                mode = STATE_START_SCREEN;
                ResetGameState();
            }
        }
    }
}

void ProcessInput(){
    switch(mode){
        case STATE_START_SCREEN:
            ProcessStartScreenInput();
            break;
        case STATE_GAME_LEVEL:
            ProcessGameLevelInput(state);
            break;
        case STATE_GAME_OVER:
            ProcessGameOverInput();
            break;
    }
}

void UpdateGameLevel(GameState &state, float elapsed){
    float distancex;
    float distancey;
    
    for(int j=0; j < MAX_BULLETS; j++) {
        state.bullets[j].position.y += state.bullets[j].velocity.y * elapsed;
        
        for(int i=0; i < 24; i++) {
            distancex = abs(state.enemies[i].position.x - state.bullets[j].position.x) - (state.enemies[i].sprite.width + state.bullets[j].sprite.width)/2;
            distancey = abs(state.enemies[i].position.y - state.bullets[j].position.y) - (state.enemies[i].sprite.height + state.bullets[j].sprite.height)/2;
            
            if(distancex < 0 && distancey < 0 && state.enemies[i].dynamic == true){
                state.enemies[i].dynamic = false;
                state.bullets[j].position.x = -2000;
                
                if(i >= 0 && i <= 5){
                    state.score += 40;
                }
                if(i >= 6 && i <= 11){
                    state.score += 30;
                }
                if(i >= 12 && i <= 17){
                    state.score += 20;
                }
                if(i >= 18 && i <= 23){
                    state.score += 10;
                }
            }
        }
    }
    
    for(int i=0; i < 24; i++) {
        state.enemies[i].position.x += state.enemies[i].velocity.x * elapsed;
        
        if ((state.enemies[i].position.x > 1.7 || state.enemies[i].position.x < -1.7) && state.enemies[i].dynamic == true){
            for(int k=0; k < 24; k++){
                state.enemies[k].velocity.x = 0 - state.enemies[k].velocity.x;
            }
        }
        
        for(int j=0; j < MAX_BULLETS; j++){
            distancex = abs(state.enemies[i].position.x - state.bullets[j].position.x) - (state.enemies[i].sprite.width + state.bullets[j].sprite.width + 0.05)/2;
            distancey = abs(state.enemies[i].position.y - state.bullets[j].position.y) - (state.enemies[i].sprite.height + state.bullets[j].sprite.height + 0.05)/2;
            
            if(distancex < 0 && distancey < 0 && state.enemies[i].dynamic == true){
                state.enemies[i].dynamic = false;
                state.bullets[j].position.x = -2000;
                
                if(i >= 0 && i <= 5){
                    state.score += 40;
                }
                if(i >= 6 && i <= 11){
                    state.score += 30;
                }
                if(i >= 12 && i <= 17){
                    state.score += 20;
                }
                if(i >= 18 && i <= 23){
                    state.score += 10;
                }
            }
        }
    }
    
    for(int j=0; j < 10; j++) {
        state.enemiesBullets[j].position.y += state.enemiesBullets[j].velocity.y * elapsed;
        
        distancex = abs(state.enemiesBullets[j].position.x - state.player.position.x) - (state.enemiesBullets[j].sprite.width + state.player.sprite.width + 0.05)/2;
        distancey = abs(state.enemiesBullets[j].position.y - state.player.position.y) - (state.enemiesBullets[j].sprite.height + state.player.sprite.height + 0.05)/2;
        
        if(distancex < 0 && distancey < 0){
            mode = STATE_GAME_OVER;
        }
    }
    
    if (state.score == 600){
        mode = STATE_GAME_OVER;
    }
    
    int randomIndex;
    
    timer += elapsed;
    if (timer > 2.0f){
        randomIndex = rand() % 23 + 0;
        while(state.enemies[randomIndex].dynamic != true){
            randomIndex = rand() % 23 + 0;
        }
        
        if(state.enemies[randomIndex].dynamic == true){
            enemyShootBullet(randomIndex);
        }
        
        timer = 0.0f;
    }
}

void Update(float elapsed){
    switch(mode){
        case STATE_START_SCREEN:
            break;
        case STATE_GAME_LEVEL:
            UpdateGameLevel(state, elapsed);
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void RenderStartScreen(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.55f, 0.0f, 0.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, textTexture, "Space Invaders", 0.145, 0.1);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3f, -0.5f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, textTexture, "Press (Enter/Return) To Begin", 0.25, 0.1);
    
    SDL_GL_SwapWindow(displayWindow);
}

void RenderGameLevel(GameState &state){
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(1.2f, 0.9f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.4f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, textTexture, std::to_string(state.score), 0.25, 0.1);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, 0.9f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, "Press (Q) to quit", 0.25, 0.1);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, state.player.position);
    program.SetModelMatrix(modelMatrix);
    state.player.sprite.Draw(program);
    
    for(int i=0; i < 24; i++) {
        if (state.enemies[i].dynamic == true){
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, state.enemies[i].position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.85f, 0.85f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            state.enemies[i].sprite.Draw(program);
        }
    }
    
    for(int i=0; i < MAX_BULLETS; i++) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, state.bullets[i].position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.65f, 0.65f, 1.0f));
        program.SetModelMatrix(modelMatrix);
        state.bullets[i].sprite.Draw(program);
    }
    
    for(int i=0; i < 10; i++) {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, state.enemiesBullets[i].position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.65f, 0.65f, 1.0f));
        program.SetModelMatrix(modelMatrix);
        state.enemiesBullets[i].sprite.Draw(program);
    }
    
    state.bullets[bulletIndex].sprite.Draw(program);
    state.enemiesBullets[enemiesBulletIndex].sprite.Draw(program);

    
    SDL_GL_SwapWindow(displayWindow);
}

void RenderGameOver(GameState &state){
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.95f, 0.0f, 0.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, textTexture, "Game Over", 0.145, 0.1);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.6f, -0.2f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);

    DrawText(program, textTexture, "Score: " + std::to_string(state.score), 0.25, 0.1);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.45f, -0.7f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, textTexture, "Press (M) to return to Start Screen", 0.25, 0.1);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Render(){
    switch(mode) {
        case STATE_START_SCREEN:
            RenderStartScreen();
            break;
        case STATE_GAME_LEVEL:
            RenderGameLevel(state);
            break;
        case STATE_GAME_OVER:
            RenderGameOver(state);
            break;
    }
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
        //Update(elapsed);
        Render();
    }
    
    Cleanup();
    return 0;
}
