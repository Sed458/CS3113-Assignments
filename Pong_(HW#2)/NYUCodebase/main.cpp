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
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    float lastFrameTicks = 0.0f;
    
    float ballpositionx = 0.0f;
    float ballpositiony = 0.0f;
    float ballMovementx = 0.75f;
    float ballMovementy = 0.75f;
    
    float paddle1y = 0.0f;
    float paddle2y = 0.0f;
    
    float elapsed = 0.0f;
    
    float distance1x = 0.0f;
    float distance1y = 0.0f;
    
    float distance2x = 0.0f;
    float distance2y = 0.0f;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    program.SetModelMatrix(modelMatrix);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
#ifdef _WINDOWS
    glewInit();
#endif

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN){
                if(keys[SDL_SCANCODE_UP]){
                    paddle2y += elapsed * 10.0f;
                }
                else if (keys[SDL_SCANCODE_DOWN]){
                    paddle2y -= elapsed * 10.0f;
                }
                if (keys[SDL_SCANCODE_W]){
                    paddle1y += elapsed * 10.0f;
                }
                else if (keys[SDL_SCANCODE_S]){
                    paddle1y -= elapsed * 10.0f;
                }
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        modelMatrix = glm::mat4(1.0f);
        
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, paddle1y, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f, 0.7f, 1.0f));
        
        program.SetModelMatrix(modelMatrix);
        
        //Paddle 1
        float vertices[] = {-0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25};
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        //Paddle 2
        modelMatrix = glm::mat4(1.0f);
        
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.6f, paddle2y, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.20f, 0.7f, 1.0f));
        
        program.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Ball
        
        modelMatrix = glm::mat4(1.0f);
        
        modelMatrix = glm::translate(modelMatrix, glm::vec3(ballpositionx, ballpositiony, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 1.0f));

        program.SetModelMatrix(modelMatrix);

        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        ballpositionx += 45.0f * (3.1415926f / 180.0f) * elapsed * ballMovementx;
        ballpositiony += 45.0f * (3.1415926f / 180.0f) * elapsed * ballMovementy;
        
        distance1x = abs(-1.6f - ballpositionx) - (0.05f + 0.05f)/2;
        distance1y = abs(paddle1y - ballpositiony) - (0.175f + 0.05f)/2;
        
        distance2x = abs(1.6f - ballpositionx) - (0.05f + 0.05f)/2;
        distance2y = abs(paddle2y - ballpositiony) - (0.175f + 0.05f)/2;
        
        if (ballpositiony >= 1.0f || ballpositiony <= -1.0f){
            ballMovementy = (0 - ballMovementy);
        }
        
        if (ballpositionx >= 1.777f){
            ballpositionx = 0.0f;
            ballpositiony = 0.0f;
            glClearColor(0.4f, 0.2f, 0.4f, 1.0f);
        }
        else if (ballpositionx <= -1.777f){
            ballpositionx = 0.0f;
            ballpositiony = 0.0f;
            glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
        }
        
        if((distance1x < 0 && distance1y < 0) || (distance2x < 0 && distance2y < 0)){
            ballMovementx = (0 - ballMovementx);
        }
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}
