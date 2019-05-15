//
//  GameState.cpp
//  NYUCodebase
//
//  Created by Sophia Danielsen on 5/7/19.
//  Copyright © 2019 Ivan Safrin. All rights reserved.
//

#include "GameState.hpp"

const Uint8 *keys = SDL_GetKeyboardState(NULL);
float quitButtonPos[] = { -0.16f, 0.16f, 0.94f, 0.79f };

void GameState::SetUp(GLuint spriteTexture, GLuint textTexture, GLuint buttonTexture, GLuint playerTexture, GLuint playerFlippedTexture, GLuint backgroundTexture){
    this->spriteTexture = spriteTexture;
    this->textTexture = textTexture;
    this->buttonTexture = buttonTexture;
    this->playerTexture = playerTexture;
    this->playerFlippedTexture = playerFlippedTexture;
    this->backgroundTexture = backgroundTexture;
    
    level = LEVEL1;
    LEVEL_HEIGHT = 19;
    LEVEL_WIDTH = 16;
    TILE_SIZE = 0.5f;
    
    lives = 3;
    gameover = false;
    timer = 0.0f;
    
    //Sprite count of map sprite sheet
    SPRITE_COUNT_X  = 30;
    SPRITE_COUNT_Y = 30;
    
    //Setup Player
    SpriteSheet pSprite(8, 4, playerTexture);
    playerSprites = pSprite;
    player.spriteSheet = playerSprites;
    player.size = glm::vec3(0.3f, 0.45f, 0.0f);
    player.currentIndex = 8;
    player.walkingIndices = {8, 9, 10, 11, 12, 13, 14, 15};
    player.velocity.x = 0.0f;
    
    SpriteSheet pFSprite(8, 4, playerFlippedTexture);
    playerFlippedSprites = pFSprite;
    
    //Setup Level Sprite Sheet (items, Entities, etc.)
    SpriteSheet sprites(30, 30, spriteTexture);
    levelSprites = sprites;
    
    player.position = glm::vec3(6.0f * TILE_SIZE, -11.5 * TILE_SIZE, 0.0f);
    
    //Setup Player Lives
    for(int i = 0; i < 3; i++){
        Entity Heart;
        Heart.spriteSheet = levelSprites;
        Heart.currentIndex = 373;
        playerHearts.push_back(Heart);
    }
    
    //Setup Coin Counter Block Sprite
    SpriteSheet ccSprite(buttonTexture, 290.0f/512.0f, 98.0f/256.0f, 49.0f/512.0f, 45.0f/256.0f, 0.2f);
    coinCountLabel = ccSprite;
    
    //Setup Instruction Label Sprite
    SpriteSheet iSprite(buttonTexture, 0.0f/512.0f, 94.0f/256.0f, 190.0f/512.0f, 45.0f/256.0f, 0.2f);
    instructionsLabel = iSprite;
    
    SpriteSheet bSprite(buttonTexture, 0.0f/512.0f, 94.0f/256.0f, 190.0f/512.0f, 45.0f/256.0f, 0.2f);
    quitButton = bSprite;
    
    ResetLevel();
}

void GameState::ResetLevel(){
    items.clear();
    enemies.clear();
    
    if(gameover == true || gameWin == true){
        level = LEVEL1;
    }

    player.hasKey = false;
    timer = 0.0f;
    randomIndex = 0;
    lives = 3;
    gameWin = false;
    gameover = false;
    
    yPos = 0.0f;
    animationTime = 0.0f;
    fallDown = true;
    
    for(int i = 0; i < 3; i++){
        playerHearts[i].currentIndex = 373;
    }
        
    ResetPlayer();
    
    switch(level){
        case LEVEL1:
            levelFile = RESOURCE_FOLDER"Level1.txt";
            openTextFile(levelFile);
            break;
        case LEVEL2:
            levelFile = RESOURCE_FOLDER"Level2.txt";
            openTextFile(levelFile);
            break;
        case LEVEL3:
            levelFile = RESOURCE_FOLDER"Level3.txt";
            openTextFile(levelFile);
            
            xVelocity = 0.5;
            
            //Create Enemies
            for(int i = 0; i < 10; i++){
                Entity entity;
                entity.position = glm::vec3(mapWidth/2*TILE_SIZE, 2*TILE_SIZE, 0.0f);
                entity.size.x = 0.45f;
                entity.size.y = 0.5f;
                entity.spriteSheet = levelSprites;
                entity.isVisible = false;
                
                entity.entityType = ENTITY_ENEMY;
                entity.currentIndex = 445;
                entity.walkingIndices = {445, 446};
                entity.velocity.x = 0.0f;
                entity.friction.x = 0.0f;
                entity.gravity.y = 0.0f;
                enemies.push_back(entity);
            }
            break;
    }
}

void GameState::ResetPlayer(){
    switch(level){
        case LEVEL1:
            player.position = glm::vec3(8.0f * TILE_SIZE, -13.5 * TILE_SIZE, 0.0f);
            break;
        case LEVEL2:
            player.position = glm::vec3(8.0f * TILE_SIZE, -15.5 * TILE_SIZE, 0.0f);
            break;
        case LEVEL3:
            player.position = glm::vec3(8.0f * TILE_SIZE, -12.5 * TILE_SIZE, 0.0f);
            break;
    }
}

void GameState::ProcessInput(SDL_Event& event){
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_1){
            level = LEVEL1;
            ResetLevel();
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_2){
            level = LEVEL2;
            ResetLevel();
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_3){
            level = LEVEL3;
            ResetLevel();
        }
        else if (event.key.keysym.scancode == SDL_SCANCODE_W){
            gameWin = true;
        }
    }
    else if (event.type == SDL_KEYUP){
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && player.collidedBottom == true){
            player.velocity.y = 6.0f;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        float mouseX = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.x : event.motion.x;
        float mouseY = (event.type == SDL_MOUSEBUTTONDOWN) ? event.button.y : event.motion.y;
        
        float unitX = (((float)mouseX / 640.0f) * 3.554f ) - 1.777f;
        float unitY = (((float)(360-mouseY) / 360.0f) * 2.0f ) - 1.0f;
        
        cout << unitX << " " << unitY << endl;
        
        if (unitX >= quitButtonPos[0] && unitX <= quitButtonPos[1] && unitY <= quitButtonPos[2] && unitY >= quitButtonPos[3]) {
            gameover = true;
        }
    }
}

void GameState::Render(ShaderProgram &program){
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    
    viewMatrix = glm::translate(viewMatrix, (glm::vec3(0.0f, 0.0f, 0.0f) - player.position));
    program.SetViewMatrix(viewMatrix);
    
    switch(level){
        case LEVEL1:
            modelMatrix = glm::mat4(1.0f);
            program.SetModelMatrix(modelMatrix);
            DrawTileMap(program);
            
            //Draw Instructions
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            instructionsLabel.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(6.75f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Collect All Coins", 0.25, 0.06);
            
            break;
        case LEVEL2:
            modelMatrix = glm::mat4(1.0f);
            program.SetModelMatrix(modelMatrix);
            DrawTileMap(program);
            
            //Draw Instructions
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.4f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            instructionsLabel.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(6.9f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Collect The Key", 0.25, 0.06);
            
            break;
        case LEVEL3:
            modelMatrix = glm::mat4(1.0f);
            program.SetModelMatrix(modelMatrix);
            DrawTileMap(program);
            
            //Draw Instructions
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(3.5f, 1.0f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            instructionsLabel.Draw(program);
            
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(6.75f * TILE_SIZE, yPos, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            DrawText(program, textTexture, "Collect All Coins", 0.25, 0.06);

            break;
    }
    
    //Draw Player
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player.position);
    modelMatrix = glm::scale(modelMatrix, player.scale);
    program.SetModelMatrix(modelMatrix);
    player.Render(program);
    
    int counter = 0;    //count number of objects not visible
    
    //Render Items (Key and Coins)
    for(int i = 0; i < items.size(); i++){
        if(items[i].isVisible == true){
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, items[i].position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            
            items[i].Render(program);
        }
        else{
            counter++;
        }
    }
    
    //Render Enemies
    for(int i = 0; i < enemies.size(); i++){
        if(enemies[i].isVisible == true){
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, enemies[i].position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
            program.SetModelMatrix(modelMatrix);
            
            enemies[i].Render(program);
        }
    }
    
    //Draw Heart
    for(int i = 0; i < 3; i++){
        modelMatrix = glm::mat4(1.0f);
        if(i == 0){
            modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(-3.25*TILE_SIZE, 1.75*TILE_SIZE, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            playerHearts[i].Render(program);
        }
        if(i == 1){
            modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(-3.0*TILE_SIZE, 1.75*TILE_SIZE, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            playerHearts[i].Render(program);
        }
        if(i == 2){
            modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(-2.75*TILE_SIZE, 1.75*TILE_SIZE, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.0f));
            program.SetModelMatrix(modelMatrix);
            playerHearts[i].Render(program);
        }
    }
    
    //Draw Coin Count Label
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(TILE_SIZE * 3.0f, TILE_SIZE * 1.5f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 2.0f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    coinCountLabel.Draw(program);
    
    modelMatrix = glm::mat4(1.0f);
    if(counter < 10){
        modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(TILE_SIZE * 3.0f, TILE_SIZE * 1.65f, 0.0f));
    }
    else{
        modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(TILE_SIZE * 2.85f, TILE_SIZE * 1.65f, 0.0f));
    }
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, to_string(counter), 0.25, 0.05);
    
    modelMatrix = glm::mat4(1.0f);
    if(items.size() < 10){
        modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(TILE_SIZE * 3.0f, TILE_SIZE * 1.35f, 0.0f));
    }
    else{
        modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(TILE_SIZE * 2.9f, TILE_SIZE * 1.35f, 0.0f));
    }
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, to_string(items.size()), 0.25, 0.05);
    
    //Draw Quit Button
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(0.0f, TILE_SIZE * 1.75f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(.75f, 0.75f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    quitButton.Draw(program);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player.position + glm::vec3(-TILE_SIZE * 0.225f, TILE_SIZE * 1.75f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 1.0f));
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textTexture, "Quit", 0.25, 0.05);
    
    //Complete Level 1
    if(counter == 28 && level == LEVEL1){
        level = LEVEL2;
        ResetLevel();
    }
    
    //Complete Level 2
    if(player.hasKey){
        level = LEVEL3;
        ResetLevel();
    }
    
    //Complete Level 3
    if(counter == 28 && level == LEVEL3){
        gameWin = true;
    }
    
    //Game Over
    if(lives == 0){
        gameover = true;
    }
    
    if(player.position.y < (-mapHeight + 2) * TILE_SIZE){
        for(int i = (int)(playerHearts.size() - 1); i >= 0; i--){
            if(playerHearts[i].currentIndex == 373){
                playerHearts[i].currentIndex = 375;
                lives--;
                ResetPlayer();
                break;
            }
        }
    }
}

void GameState::Update(float elapsed){
    //Animate Instructions Label
    float valueY = 0.0f;
    switch(level){
        case LEVEL1:
            valueY = -13.0f * TILE_SIZE;
            break;
        case LEVEL2:
            valueY = -15.0f * TILE_SIZE;
            break;
        case LEVEL3:
            valueY = -12.0f * TILE_SIZE;
            break;
    }
    
    animationTime = animationTime + elapsed;
    float animationValue = mapValue(animationTime, 0.0f, 1.0f, 0.0f, 1.0f);

    yPos = easeIn(-mapHeight * TILE_SIZE, valueY, animationValue);
    
    if (fallDown) {
        fallDown = false;
        animationTime = 0.0f;
    }
    
    //Reset Player Collisions to False
    player.resetCollisions();
    
    //Player Animation (Walking)
    float framesPerSecond = 30.0f;
    if(keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT]){
        player.animationElapsed += elapsed;
        if(player.animationElapsed > 1.0/framesPerSecond) {
            player.currentIndex++;
            player.animationElapsed = 0.0;
            if(player.currentIndex > 15) {
                player.currentIndex = 8;
            }
        }
    }
    else{
        player.currentIndex = 8;
    }
    
    //Animate Ghosts (enemies)
    framesPerSecond = 2.0f;
    for(int i = 0; i < enemies.size(); i++){
        enemies[i].animationElapsed += elapsed;
        if(enemies[i].animationElapsed > 1.0/framesPerSecond) {
            enemies[i].currentIndex++;
            enemies[i].animationElapsed = 0.0;
            if(enemies[i].currentIndex > 446) {
                enemies[i].currentIndex = 445;
            }
        }
    }
    
    //Check For Player Input
    if (keys[SDL_SCANCODE_LEFT]){
        player.acceleration.x = -2.0f;
        player.spriteSheet = playerFlippedSprites;
    }
    else if (keys[SDL_SCANCODE_RIGHT]){
        player.acceleration.x = 2.0f;
        player.spriteSheet = playerSprites;
    }
    
    //Player Velocity/Acceleration Update
    player.setVelocity(elapsed);
    player.position.y += player.velocity.y * elapsed;
    collisionY(player);
    player.position.x += player.velocity.x * elapsed;
    collisionX(player);
    
    player.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
    checkObjectCollision();
    
    if(level == LEVEL3){
        timer += elapsed;
        if (timer > 5.0f){
            randomIndex = rand() % 9 + 0;
            while(enemies[randomIndex].gravity.y == -9.8f){
                randomIndex = rand() % 9 + 0;
            }
            
            if(enemies[randomIndex].gravity.y != -9.8f){
                xVelocity = 0 - xVelocity;
                enemies[randomIndex].velocity.x = xVelocity;
                enemies[randomIndex].gravity.y = -9.8f;
                enemies[randomIndex].isVisible = true;
            }
            
            cout << randomIndex << endl;
            timer = 0.0f;
        }
    }
    
    
    
    for(int i = 0; i < enemies.size(); i++){
        if(enemies[i].isVisible){
            enemies[i].resetCollisions();
            enemies[i].setVelocity(elapsed);
            enemies[i].position.y += enemies[i].velocity.y * elapsed;
            collisionY(enemies[i]);
            enemies[i].position.x += enemies[i].velocity.x * elapsed;
            collisionX(enemies[i]);
            if(level == LEVEL1 || level == LEVEL2){
                BeforeCollisionX(enemies[i]);
            }
            if(enemies[i].collidedLeft){
                enemies[i].velocity.x = 0.5f;
            }
            else if(enemies[i].collidedRight){
                enemies[i].velocity.x = -0.5f;
            }
        }
        
        if(enemies[i].position.y < ((-mapHeight + 2) * TILE_SIZE)){
            enemies[i].position = glm::vec3(mapWidth/2*TILE_SIZE, 2*TILE_SIZE, 0.0f);
            enemies[i].isVisible = false;
            
            enemies[i].velocity.x = 0.0f;
            enemies[i].velocity.y = 0.0f;
            enemies[i].gravity.y = 0.0f;

        }
    }
}

//Drawing Level Maps
void GameState::DrawTileMap(ShaderProgram& program){
    vector<float> vertexData;
    vector<float> texCoordData;
    
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
    
    glBindTexture(GL_TEXTURE_2D, spriteTexture);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)(vertexData.size()/2));
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void GameState::placeEntity(string type, float placeX, float placeY){
    Entity entity;
    
    entity.position = glm::vec3(placeX + TILE_SIZE/2, placeY + TILE_SIZE/2, 0.0f);
    entity.size.x = 0.5f;
    entity.size.y = 0.5f;
    entity.spriteSheet = levelSprites;
    entity.isVisible = true;
    
    if(type == "ENTITY_ENEMY"){
        entity.entityType = ENTITY_ENEMY;
        entity.size.x = 0.45f;
        entity.currentIndex = 445;
        entity.walkingIndices = {445, 446};
        entity.velocity.x = 0.5f;
        entity.friction.x = 0.0f;
        enemies.push_back(entity);
    }
    else if(type == "ITEM_COIN"){
        entity.itemType = ITEM_COIN;
        entity.currentIndex = 78;
        items.push_back(entity);
    }
    else if(type == "ITEM_KEY"){
        entity.itemType = ITEM_KEY;
        entity.currentIndex = 45;
        items.push_back(entity);
    }
}

bool GameState::readHeader(std::ifstream &stream) {
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
        levelData = new unsigned int*[mapHeight];
        solidData = new unsigned int*[mapHeight];
        
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned int[mapWidth];
            solidData[i] = new unsigned int[mapWidth];
        }
        return true;
    }
}

bool GameState::readLayerData(std::ifstream &stream) {
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
                    unsigned int val =  (unsigned int)atoi(tile.c_str());
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

bool GameState::readEntityData(std::ifstream &stream) {
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

void GameState::openTextFile(string filename){
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
            
        } else if(line == "[ObjectLayer]") {
            readEntityData(infile);
        }
    }
}

void GameState::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}

bool GameState::checkTileCollision(Entity &entity, int checkX, int checkY, int tileX, int tileY){
    //Check if tile coordinates are negative or larger than map
    if (checkX < 0) return false;
    if (checkY < 0) return false;
    if (checkX >= mapWidth) return false;
    if (checkY >= mapHeight) return false;
    
    bool collision = false;
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
    
    if (solidData[checkY][checkX] == 1){
        float posX = (checkX * TILE_SIZE);
        float posY = (-checkY * TILE_SIZE);
        
        if (entity.position.y - entity.size.y/2 <= posY){
            bottom = true;   //Bottom
        }
        if (entity.position.y + entity.size.y/2 >= posY - TILE_SIZE){
            top = true;      //Top
        }
        if (entity.position.x + entity.size.x/2 <= posX + TILE_SIZE){
            left = true;     //Left
        }
        if (entity.position.x - entity.size.x/2 >= posX){
            right = true;    //Right
        }
        
        collision = (top | bottom | left | right);
    }
    return collision;
}

void GameState::collisionY(Entity &entity){
    //Bottom
    worldToTileCoordinates(entity.position.x, (entity.position.y - entity.size.y/2) - 0.0125, &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(collision){
        entity.velocity.y = 0;
        entity.position.y += 0.0125 + (-(gridY) * TILE_SIZE) - (entity.position.y - entity.size.y/2);
        entity.collidedBottom = true;
        collision = false;
    }
    
    //Top
    worldToTileCoordinates(entity.position.x, (entity.position.y + entity.size.y/2 - 0.2), &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(collision){
        entity.velocity.y = 0;
        entity.position.y += 0.2 + (entity.position.y + entity.size.y/2) - (-(gridY) * TILE_SIZE);
        entity.collidedTop = true;
        collision = false;
    }
}

void GameState::collisionX(Entity &entity){
    //Left
    worldToTileCoordinates((entity.position.x - entity.size.x/2), entity.position.y, &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(collision){
        entity.velocity.x = 0;
        entity.position.x += 0.1 + (entity.position.x - entity.size.x/2) - (((gridX) * TILE_SIZE) + TILE_SIZE);
        entity.collidedLeft = true;
        collision = false;
    }
    
    //Right
    worldToTileCoordinates((entity.position.x + entity.size.x/2), entity.position.y, &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(collision){
        entity.velocity.x = 0;
        entity.position.x -= (entity.position.x + entity.size.x/2) - ((gridX) * TILE_SIZE);
        entity.collidedRight = true;
        collision = false;
    }
}

//Enemy checks for end of platform (empty tile)
void GameState::BeforeCollisionX(Entity &entity){
    //Left
    worldToTileCoordinates((entity.position.x - entity.size.x/2)-TILE_SIZE/4, entity.position.y - TILE_SIZE, &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(!collision){
        entity.velocity.x = 0;
        entity.position.x += 0.1 + (entity.position.x - entity.size.x/2-TILE_SIZE/4) - (((gridX) * TILE_SIZE) + TILE_SIZE);
        entity.collidedLeft = true;
        collision = false;
    }
    
    //Right
    worldToTileCoordinates((entity.position.x + entity.size.x/2)+TILE_SIZE/4, entity.position.y - TILE_SIZE, &gridX, &gridY);
    collision = checkTileCollision(entity, gridX, gridY, gridX, gridY);
    if(!collision){
        entity.velocity.x = 0;
        entity.position.x -= (entity.position.x + entity.size.x/2+TILE_SIZE/4) - ((gridX) * TILE_SIZE);
        entity.collidedRight = true;
        collision = false;
    }
}

void GameState::checkObjectCollision(){
    float distancex;
    float distancey;
    
    for(int j=0; j < items.size(); j++) {
        distancex = abs(player.position.x - items[j].position.x) - (items[j].size.x + player.size.x)/2 + 0.15;
        distancey = abs(items[j].position.y - player.position.y) - (items[j].size.y + player.size.y)/2 + 0.15;
        
        if(distancex < 0 && distancey < 0){
            if(items[j].isVisible){
                Mix_VolumeChunk(coinCollect, 20);
                Mix_PlayChannel(-1, coinCollect, 0);
            }
            
            items[j].isVisible = false;
            
            if(items[j].itemType == ITEM_KEY){
                player.hasKey = true;
            }
        }
    }
    
    for(int j=0; j < enemies.size(); j++) {
        distancex = abs(player.position.x - enemies[j].position.x) - (enemies[j].size.x + player.size.x)/2 + 0.15;
        distancey = abs(enemies[j].position.y - player.position.y) - (enemies[j].size.y + player.size.y)/2 + 0.15;
    
        if(distancex < 0 && distancey < 0){
            Mix_VolumeChunk(hitEnemy, 20);
            Mix_PlayChannel(-1, hitEnemy, 0);
            
            for(int i = 0; i < playerHearts.size(); i++){
                if(playerHearts[i].currentIndex == 373){
                    playerHearts[i].currentIndex = 375;
                    lives--;
                    ResetPlayer();
                    break;
                }
            }
        }
    }
}

bool GameState::getGameOver(){
    return gameover;
}

bool GameState::getGameWin(){
    return gameWin;
}
