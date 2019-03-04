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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath){
    int w, h, comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(image);
    return retTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    ShaderProgram texturedProgram;
    texturedProgram.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    GLuint pinkAlienTexture = LoadTexture(RESOURCE_FOLDER"assets/alienPink.png");
    GLuint grassTexture = LoadTexture(RESOURCE_FOLDER"assets/grass.png");
    GLuint cloud1Texture = LoadTexture(RESOURCE_FOLDER"assets/cloud1.png");
    GLuint cloud2Texture = LoadTexture(RESOURCE_FOLDER"assets/cloud2.png");
    GLuint coinDiamondTexture = LoadTexture(RESOURCE_FOLDER"assets/coinDiamond.png");
    GLuint starGoldTexture = LoadTexture(RESOURCE_FOLDER"assets/starGold.png");
    
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Untextured Polygons
        glUseProgram(program.programID);
        
        modelMatrix = glm::mat4(1.0f);
        
        //Small Triangle
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f, -0.60f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.60f, 1.0f));
        
        program.SetModelMatrix(modelMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        float vertices[] = {0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        program.SetColor(0.3f, 0.25f, 0.3f, 1.0f);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        //Draw Rectangle
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.3f, -0.60f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f, 0.6f, 1.0f));
        
        program.SetModelMatrix(modelMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        float vertices2[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
        glEnableVertexAttribArray(program.positionAttribute);
        
        program.SetColor(0.3f, 0.25f, 0.3f, 1.0f);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        //Draw Smaller Triangle
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.6f, -0.2f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.35f, 1.0f));
        
        program.SetModelMatrix(modelMatrix);
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        float vertices11[] = {0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices11);
        glEnableVertexAttribArray(program.positionAttribute);
        
        program.SetColor(0.3f, 0.25f, 0.3f, 1.0f);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        //Textured Polygons
        glUseProgram(texturedProgram.programID);
        
        //Draw Pink Alien
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3f, -0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.65f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        texturedProgram.SetProjectionMatrix(projectionMatrix);
        texturedProgram.SetViewMatrix(viewMatrix);
        
        glBindTexture(GL_TEXTURE_2D, pinkAlienTexture);
        float vertices3[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Grass
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 0.5f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        float vertices4[] = {-2.0, -0.5, 2.0, -0.5, 2.0, 0.5, -2.0, -0.5, 2.0, 0.5, -2.0, 0.5};
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords2[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Cloud 1
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.75f, 0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 0.5f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, cloud1Texture);
        float vertices5[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords3[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT,false, 0, texCoords3);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Cloud 2
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.325f, 0.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.65f, 0.4f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, cloud2Texture);
        float vertices6[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices6);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords4[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Diamond Coin 1
        
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.75f, -0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, coinDiamondTexture);
        float vertices7[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices7);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords5[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords5);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Diamond Coin 2
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f, -0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, coinDiamondTexture);
        float vertices8[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices8);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords6[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords6);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Diamond Coin 3
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, -0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, coinDiamondTexture);
        float vertices9[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices9);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords7[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords7);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        //Draw Star
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1.15f, -0.1f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 1.0f));
        
        texturedProgram.SetModelMatrix(modelMatrix);
        
        glBindTexture(GL_TEXTURE_2D, starGoldTexture);
        float vertices10[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(texturedProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices10);
        glEnableVertexAttribArray(texturedProgram.positionAttribute);
        
        float texCoords8[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(texturedProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords8);
        glEnableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texturedProgram.positionAttribute);
        glDisableVertexAttribArray(texturedProgram.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
