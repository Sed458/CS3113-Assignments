//
//  GameLevel.cpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#include "GameLevel.hpp"

GameLevel::GameLevel () {
    mode = STATE_START_SCREEN;
}

void GameLevel::SetUp(){
    startScreen.Setup(textTexture, buttonTexture);
    level.SetUp(spriteTexture, textTexture, buttonTexture, playerTexture, playerFlippedTexture, backgroundTexture);
    
    quitGame = false;
    
    SpriteSheet bSprite(backgroundTexture, 0.0f/640.0f, 0.0f/360.0f, 640.0f/640.0f, 360.0f/360.0f, 1.0f);
    background = bSprite;
    
    SpriteSheet lbSprite(levelTexture, 0.0f/640.0f, 0.0f/360.0f, 640.0f/640.0f, 360.0f/360.0f, 1.0f);
    levelBackground = lbSprite;
    
    SpriteSheet paSprite(buttonTexture, 0.0f/512.0f, 45.0f/256.0f, 190.0f/512.0f, 49.0f/256.0f, 0.2f);
    playAgainButton = paSprite;
    
    SpriteSheet mSprite(buttonTexture, 0.0f/512.0f, 45.0f/256.0f, 190.0f/512.0f, 49.0f/256.0f, 0.2f);
    menuButton = mSprite;
    
    SpriteSheet qSprite(buttonTexture, 380.0f/512.0f, 36.0f/256.0f, 38.0f/512.0f, 36.0f/256.0f, 0.2f);
    quitButton = qSprite;
}

void GameLevel::SetUpSound(){
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    music = Mix_LoadMUS(RESOURCE_FOLDER "DungeonTheme.mp3");
    Mix_VolumeMusic(30);
    Mix_PlayMusic(music, -1);
    
    level.coinCollect = Mix_LoadWAV(RESOURCE_FOLDER "CoinCollect.wav");
    level.hitEnemy = Mix_LoadWAV(RESOURCE_FOLDER "HitEnemy.wav");
}

void GameLevel::Render(ShaderProgram &program){
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    switch(mode){
        case STATE_START_SCREEN:
            //Draw Background
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 2.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            background.Draw(program);
            
            //Draw Quit Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, 0.85f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            quitButton.Draw(program);
            
            startScreen.Render(program);
            break;
        case STATE_GAME_LEVEL:
            //Draw Background
            modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f * 0.5f, -8.0f*0.5, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 11.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            levelBackground.Draw(program);
            
            level.Render(program);
            break;
        case STATE_YOU_WIN:
            viewMatrix = glm::mat4(1.0f);
            program.SetViewMatrix(viewMatrix);
            
            //Draw Background
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 2.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            background.Draw(program);
            
            //Draw "GAME OVER"
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.75f, 0.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            
            DrawText(program, textTexture, "You Win!", 0.145, 0.1);
            
            //Draw Play Again Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.4f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            playAgainButton.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.44f, -0.4f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Play Again?", 0.25, 0.1);
            
            //Draw Menu Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.75f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            playAgainButton.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.34f, -0.75f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Main Menu", 0.25, 0.1);
            
            //Draw Quit Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, 0.85f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            quitButton.Draw(program);
            
            break;
        case STATE_GAME_OVER:
            viewMatrix = glm::mat4(1.0f);
            program.SetViewMatrix(viewMatrix);
            
            //Draw Background
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 2.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            background.Draw(program);
            
            //Draw "GAME OVER"
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.95f, 0.0f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            
            DrawText(program, textTexture, "Game Over", 0.145, 0.1);
            
            //Draw Play Again Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.4f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            playAgainButton.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.44f, -0.4f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Play Again?", 0.25, 0.1);
            
            //Draw Menu Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.75f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            playAgainButton.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.34f, -0.75f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Main Menu", 0.25, 0.1);
            
            //Draw Quit Button
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.6f, 0.85f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            quitButton.Draw(program);
            
            break;
    }
}

void GameLevel::Update(float elapsed){
    if(startScreen.GameStart()){
        mode = STATE_GAME_LEVEL;
    }
    
    if(level.getGameWin()){
        mode = STATE_YOU_WIN;
    }
    
    if(level.getGameOver()){
        mode = STATE_GAME_OVER;
    }
    
    switch(mode){
        case STATE_START_SCREEN:
            startScreen.Update(elapsed);
            break;
        case STATE_GAME_LEVEL:
            level.Update(elapsed);
            break;
        case STATE_YOU_WIN:
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void GameLevel::ProcessInput(SDL_Event &event){            
    float playAgainButtonPos[] = { -0.67f, 0.67f, -0.26f, -0.56f };
    float menuButtonPos[] = { -0.67f, 0.67f, -0.61f, -0.91f };
    float quitButtonPos[] = { -1.69f, -1.51f, 0.93f, 0.74f };
    
    switch(mode){
        case STATE_START_SCREEN:
            startScreen.ProcessInput(event);
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                float mouseX = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.x : event.motion.x;
                float mouseY = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.y : event.motion.y;
                
                float unitX = (((float)mouseX / 640.0f) * 3.554f ) - 1.777f;
                float unitY = (((float)(360-mouseY) / 360.0f) * 2.0f ) - 1.0f;
                
                if (unitX >= quitButtonPos[0] && unitX <= quitButtonPos[1] && unitY <= quitButtonPos[2] && unitY >= quitButtonPos[3]) {
                    quitGame = true;
                }
            }
            break;
        case STATE_GAME_LEVEL:
            level.ProcessInput(event);
            break;
        case STATE_YOU_WIN:
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                float mouseX = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.x : event.motion.x;
                float mouseY = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.y : event.motion.y;
                
                float unitX = (((float)mouseX / 640.0f) * 3.554f ) - 1.777f;
                float unitY = (((float)(360-mouseY) / 360.0f) * 2.0f ) - 1.0f;
                
                cout << unitX << " " << unitY << endl;
                
                if (unitX >= playAgainButtonPos[0] && unitX <= playAgainButtonPos[1] && unitY <= playAgainButtonPos[2] && unitY >= playAgainButtonPos[3]) {
                    mode = STATE_GAME_LEVEL;
                    level.SetUp(spriteTexture, textTexture, buttonTexture, playerTexture, playerFlippedTexture, backgroundTexture);
                }
                if (unitX >= menuButtonPos[0] && unitX <= menuButtonPos[1] && unitY <= menuButtonPos[2] && unitY >= menuButtonPos[3]) {
                    mode = STATE_START_SCREEN;
                    startScreen.Setup(textTexture, buttonTexture);
                    level.SetUp(spriteTexture, textTexture, buttonTexture, playerTexture, playerFlippedTexture, backgroundTexture);
                }
                if (unitX >= quitButtonPos[0] && unitX <= quitButtonPos[1] && unitY <= quitButtonPos[2] && unitY >= quitButtonPos[3]) {
                    quitGame = true;
                }
            }
            break;
        case STATE_GAME_OVER:
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                float mouseX = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.x : event.motion.x;
                float mouseY = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.y : event.motion.y;
                
                float unitX = (((float)mouseX / 640.0f) * 3.554f ) - 1.777f;
                float unitY = (((float)(360-mouseY) / 360.0f) * 2.0f ) - 1.0f;
                
                cout << unitX << " " << unitY << endl;
                
                if (unitX >= playAgainButtonPos[0] && unitX <= playAgainButtonPos[1] && unitY <= playAgainButtonPos[2] && unitY >= playAgainButtonPos[3]) {
                    mode = STATE_GAME_LEVEL;
                    level.SetUp(spriteTexture, textTexture, buttonTexture, playerTexture, playerFlippedTexture, backgroundTexture);
                }
                if (unitX >= menuButtonPos[0] && unitX <= menuButtonPos[1] && unitY <= menuButtonPos[2] && unitY >= menuButtonPos[3]) {
                    mode = STATE_START_SCREEN;
                    startScreen.Setup(textTexture, buttonTexture);
                    level.SetUp(spriteTexture, textTexture, buttonTexture, playerTexture, playerFlippedTexture, backgroundTexture);
                }
                if (unitX >= quitButtonPos[0] && unitX <= quitButtonPos[1] && unitY <= quitButtonPos[2] && unitY >= quitButtonPos[3]) {
                    quitGame = true;
                }
            }
            break;
    }
}

bool GameLevel::getQuitGame(){
    return quitGame;
}
