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

#include "SpriteSheet.hpp"
#include "GameLevel.hpp"
#include "UtilityFunctions.hpp"
#include "GameState.hpp"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// 60 FPS (1.0f/60.0f) (update sixty times a second)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;
ShaderProgram program;
glm::mat4 projectionMatrix;
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
bool done;
float lastFrameTicks;
float ticks;
float elapsed;
float accumulator;
GLuint textTexture;
GLuint spriteSheetTexture;
GLuint playerTexture;

GameLevel level;

SDL_Event event;

void SetupTexture(){
    level.textTexture = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
    level.buttonTexture = LoadTexture(RESOURCE_FOLDER"redSheet.png");
    level.spriteTexture = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");
    level.playerTexture = LoadTexture(RESOURCE_FOLDER"characters_3.png");
    level.playerFlippedTexture = LoadTexture(RESOURCE_FOLDER"characters_3_flip.png");
    level.backgroundTexture = LoadTexture(RESOURCE_FOLDER"background.png");
    level.levelTexture = LoadTexture(RESOURCE_FOLDER"levelBackground.png");
}

void Setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Final Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    //Setup Textures (font, buttons, sprites)
    SetupTexture();
    level.SetUp();
    level.SetUpSound();
    
    projectionMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetModelMatrix(modelMatrix);
    
    lastFrameTicks = 0.0f;
    elapsed = 0.0f;
    accumulator = 0.0f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(program.programID);
}

void ProcessInput(SDL_Event& event){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        level.ProcessInput(event);
    }
}

void Update(float elapsed){
    if(level.getQuitGame()){
        done = true;
    }
    
    level.Update(elapsed);
}

void RenderGameOver(GameState &state){
    glClear(GL_COLOR_BUFFER_BIT);
    
    modelMatrix = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.95f, 0.0f, 0.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, level.textTexture, "Game Over", 0.145, 0.1);
    
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.6f, -0.2f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    //DrawText(program, level.textTexture, "Score: " + std::to_string(state.score), 0.25, 0.1);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.45f, -0.7f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    
    DrawText(program, level.textTexture, "Press (M) to return to Start Screen", 0.25, 0.1);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Render(){
    level.Render(program);
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
        
        ProcessInput(event);
        Render();
    }
    
    Cleanup();
    return 0;
}
