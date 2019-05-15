//
//  GameLevel.hpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/5/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#ifndef GameLevel_hpp
#define GameLevel_hpp

#include <stdio.h>
#include <SDL_mixer.h>
#include "ShaderProgram.h"
#include "Entity.hpp"
#include "StartScreen.hpp"
#include "GameState.hpp"
#include "SpriteSheet.hpp"

enum GameMode {STATE_START_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER, STATE_YOU_WIN};

class GameLevel {
public:
    GameLevel();
    void SetUp();
    void SetUpSound();
    void Render(ShaderProgram &program);
    void Update(float elapsed);
    void ProcessInput(SDL_Event &event);
    
    bool getQuitGame();
    
    GLuint textTexture;
    GLuint buttonTexture;
    GLuint spriteTexture;
    GLuint playerTexture;
    GLuint playerFlippedTexture;
    GLuint backgroundTexture;
    GLuint levelTexture;
    
private:
    bool quitGame;
    
    GameMode mode;
    GameState level;
    StartScreen startScreen;
    
    Mix_Music *music;
    
    SpriteSheet background;
    SpriteSheet levelBackground;
    SpriteSheet playAgainButton;
    SpriteSheet menuButton;
    SpriteSheet quitButton;
};

#endif /* GameLevel_hpp */
