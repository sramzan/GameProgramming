//
//  space-invaders.cpp
//
//
//  Created by Sean Ramzan on 2/27/16.
//
//
/*
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "Matrix.h"
#include "ShaderProgram.h"

#include "../programGenerator.cpp"
#include "windowGenerator.cpp"

#include "../global-variables.cpp"
#include "sheetSprite.cpp"

#include <vector>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

float r = .4f;
float g = .3f;
float b = .5f;
float a = 1.0f;

bool gameEnded = false;
const int START_SCREEN = 1;
const int GAME_SCREEN  = 2;
const int END_SCREEN   = 3;
int GAME_STATE   = START_SCREEN;

void blendTextures(GLuint texture){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_TEXTURE_2D);
}

void DrawText(ShaderProgram *program, int fontTexture, string text, float size, float spacing, float x_offset, float y_offset) {
    float texture_size = 1.0/16.0f;
    vector<float> vertexData;
    vector<float> texCoordData;
    for(int i=0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size) + x_offset  , 0.5f * size + y_offset,
            ((size+spacing) * i) + (-0.5f * size) + x_offset,  -0.5f * size + y_offset,
            ((size+spacing) * i) + (0.5f * size)  + x_offset,   0.5f * size + y_offset,
            
            ((size+spacing) * i) + (0.5f * size)  + x_offset, -0.5f * size + y_offset,
            ((size+spacing) * i) + (0.5f * size)  + x_offset,  0.5f * size + y_offset,
            ((size+spacing) * i) + (-0.5f * size) + x_offset, -0.5f * size + y_offset,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }

    blendTextures(fontTexture);
    
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    

}


GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}

class enemy : public ScreenObject{
public:
    enemy (ShaderProgram* program, GLuint spriteSheetTexture) : ScreenObject(program){
        this->enemySheetSprite = new SheetSprite(spriteSheetTexture, 423.0f/1024.0f, 728.0f/1024.0f, 93.0f/1024.0f, 84.0f/1024.0f, 1.0);
        setPositionInSpace(enemySheetSprite->getTopPos(), enemySheetSprite->getBottomPos(), enemySheetSprite->getLeftPos(), enemySheetSprite->getRightPos());
        setProjection();
        setMatrices();
    }
    
    void isHitWall(){
        
    }
    
    void destroy(){
        delete enemySheetSprite;
        delete this;
    }
    
    void draw(){
        setModelMatrix();
        enemySheetSprite->Draw(this);
    }
    
    float getSheetSpriteSize() { return enemySheetSprite->getSize(); }
    
private:
    SheetSprite* enemySheetSprite;
    
};

class bullet : public ScreenObject{
public:
    bullet(ShaderProgram* program, GLuint spriteSheetTexture) : ScreenObject(program){
        this->bulletSheetSprite = new SheetSprite(spriteSheetTexture, 845.0f/1024.0f, 0.0f/1024.0f, 13.0f/1024.0f, 57.0f/1024.0f, 1.0);
        setPositionInSpace(bulletSheetSprite->getTopPos(), bulletSheetSprite->getBottomPos(), bulletSheetSprite->getLeftPos(), bulletSheetSprite->getRightPos());
        setProjection();
        setMatrices();
        
    }
    
    void destroy(){
        delete bulletSheetSprite;
        delete this;
    }
    
    void draw(){
        //        setModelMatrix();
        bulletSheetSprite->Draw(this);
    }
    
    
    bool isOffScreen(){
        // if (bullet Top pos >= topBoundary of the ortho projection, then delete it from the array)
        if (getTopPos() >= getTopBoundary())
            return true;
        return false;
    }
    
    float getSheetSpriteSize(){
        return bulletSheetSprite->getSize();
    }
    
private:
    SheetSprite* bulletSheetSprite;
    
};

class player : public ScreenObject{
public:
    player (ShaderProgram* program) : ScreenObject(program){
        this->playerShipSheetSprite = new SheetSprite(this->spriteSheetTexture, 0.0f/1024.0f, 941.0f/1024.0f, 112.0f/1024.0f, 75.0f/1024.0f, 1.0);
        setPositionInSpace(playerShipSheetSprite->getTopPos(), playerShipSheetSprite->getBottomPos(), playerShipSheetSprite->getLeftPos(),playerShipSheetSprite->getRightPos());
        setProjection();
        setMatrices();
    }
    
    void fire(Mix_Chunk *laserBlastSound){
        bullets.push_back(newBullet());
        Mix_PlayChannel( -1, laserBlastSound, 0);
    }
    
    bullet* newBullet(){
        bullet* newBullet = new bullet(this->program, this->spriteSheetTexture);
        newBullet->setPosition(getLeftPos()+.75, getTopPos() + .5, 0, newBullet->getSheetSpriteSize());
        return newBullet;
    }
    
    void moveFiredBulletsUp(float elapsed){
        if (bullets.size() == 0)
            return;
        
        for (int pos = 0; pos < bullets.size(); pos++){
            bullet* currentBullet = bullets[pos];
            currentBullet->moveUp(10 * elapsed);
            processEnemyBulletCollisions();
            if (currentBullet->isOffScreen()){
                bullets.erase(bullets.begin() + pos);
                currentBullet->destroy();
                continue;
            }
            currentBullet->draw();
        }
    }
    
    void draw(){
        setModelMatrix();
        playerShipSheetSprite->Draw(this);
    }
    
    void destroy(){
        for (int pos = 0; pos < bullets.size(); pos++){
            bullet* currentBullet = bullets[pos];
            currentBullet->destroy();
        }
        
        delete this;
    }
    
    void processMovement(const Uint8* keys, float elapsed){
        if (keys[SDL_SCANCODE_RIGHT] && getRightPos() <= getRightBoundary())
            moveToTheRight(5 * elapsed);
        if (keys[SDL_SCANCODE_LEFT] && getLeftPos() >= getLeftBoundary())
            moveToTheLeft(-5 * elapsed);
    }
    
    void processEnemyBulletCollisions(){
        for (int bulletIndex = 0; bulletIndex < bullets.size(); bulletIndex++){
            bullet* currentBullet = bullets[bulletIndex];
            
            for (int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++){
                enemy* currentEnemy= enemies[enemyIndex];
                
                if ( (currentBullet->getTopPos() >= currentEnemy->getBottomPos()) &&
                    (currentBullet->getRightPos() <= (currentEnemy->getRightPos() + .75))  &&
                    (currentBullet->getLeftPos() >= (currentEnemy->getLeftPos()) - .75)){
                    enemies.erase(enemies.begin() + enemyIndex);
                    currentEnemy->destroy();
                }
            }
        }
        
        //        drawEnemies();
    }
    
    void moveAllEnemiesDown(float elapsed){
        for (enemy* currentEnemy : enemies){
            //             currentEnemy->setPosition(currentEnemy->getLeftPos(), currentEnemy->getTopPos()-.5, 0, currentEnemy->getSheetSpriteSize());
            currentEnemy->moveDown(-1);
        }
    }
    
    void moveAllEnemiesTowardsWall(float elapsed){
        if (direction == 1){
            for (int enemyIndex = enemies.size() - 1; enemyIndex >= 0; enemyIndex--){
                enemy* currentEnemy = enemies[enemyIndex];
                currentEnemy->move(1 * direction * elapsed, 0, 0);
                //                currentEnemy->setPosition(currentEnemy->getLeftPos() + 1, currentEnemy->getTopPos(), 0, currentEnemy->getSheetSpriteSize());
                if (enemies[enemies.size() - 1]->getRightPos() >= getRightBoundary()){
                    moveAllEnemiesDown(elapsed);
                    direction = -1;
                    return;
                }
            }
        }
        
        else
            for (int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++){
                enemy* currentEnemy = enemies[enemyIndex];
                currentEnemy->move(1.5 * direction * elapsed, 0, 0);
                //                currentEnemy->setPosition(currentEnemy->getLeftPos() - 1, currentEnemy->getTopPos(), 0, currentEnemy->getSheetSpriteSize());
                if (enemies[0]->getLeftPos() <= getLeftBoundary() + 1){
                    moveAllEnemiesDown(elapsed);
                    direction = 1;
                    return;
                }
            }
    }
    
    enemy* newEnemy(float xpos, float ypos, float zpos){
        enemy* newEnemy = new enemy(this->program, this->spriteSheetTexture);;
        newEnemy->setPosition(xpos, ypos, zpos, newEnemy->getSheetSpriteSize());
        return newEnemy;
    }
    
    void addEnemies(){
        // For now, this will only be called when the game begins/restarts
        
        float enemyXpos = getLeftBoundary();
        float enemyYpos = getTopBoundary() - 1;
        int newEnemyIndex = 0;
        
        while (enemyXpos + 1.5 <= getRightBoundary()){
            enemies.push_back(newEnemy(enemyXpos + 1.5, enemyYpos, 0));
            enemyXpos += enemies[newEnemyIndex]->getSheetSpriteSize() + 1.5;
            enemies[newEnemyIndex]->draw();
            newEnemyIndex++;
        }
    }
    
    void drawEnemies(){
        for (int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++){
            enemy* currentEnemy= enemies[enemyIndex];
            currentEnemy->draw();
        }
        
    }
    bool enemyIsBelowPlayer(){
        if (enemies.size() > 0){
            if (enemies[0]->getBottomPos() <= getTopPos())
                return true;
        }
        
        return false;
    }
    
    void deleteAllEnemies(){
        for (enemy* currentEnemy : enemies)
            currentEnemy->destroy();
    }
    
    int getnumEnemies() { return enemies.size(); }
    
    size_t getNumBullets(){
        return bullets.size();
    }
    
    
private:
    vector<bullet*> bullets;
    vector<enemy*> enemies;
    SheetSprite* playerShipSheetSprite;
    float direction = 1;
    GLuint spriteSheetTexture = LoadTexture("spacePics.png");
    
};

void updateGame(Program* program, player* player1, Window* window, float fixedElapsed, const Uint8* keys){
    program->clearScreen(r,g,b,a);
    player1->processMovement(keys, fixedElapsed);
    player1->drawEnemies();
    player1->draw();
    player1->moveFiredBulletsUp(fixedElapsed);
    player1->moveAllEnemiesTowardsWall(fixedElapsed);
    player1->processEnemyBulletCollisions();
    // handle moving all bullets consistently up the screen
    
    SDL_GL_SwapWindow(window->getDispWindow());
}


void playGame(Program* program, Window* window, Mix_Chunk* laserBlastSound){
    
    player* player1 = new player(program->getShaderProgram());
    cout << "Bottom Boundary: " << player1->getBottomBoundary() << endl << "Bottom Pos: " << player1->getBottomPos() << endl << "Top Pos: " << player1->getTopPos() << endl << "Top Boundary: " << player1->getTopBoundary();
    
    player1->addEnemies();
    player1->moveDown(-fabs(player1->getBottomPos() - player1->getBottomBoundary()));
    SDL_Event event;
    bool done = false;
    float lastFrameTicks = 0.0f;
    
    while (!done) {
        float ticks    = (float)SDL_GetTicks()/1000.0f;
        float elapsed  = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        if(player1->getnumEnemies() == 0 || player1->enemyIsBelowPlayer()){
            program->clearScreen(r,g,b,a);
            player1->deleteAllEnemies();
            player1->destroy();
            gameEnded = true;
            break;
        }
        
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        
        if(keys[SDL_SCANCODE_SPACE]){ // create new bullet
            player1->fire(laserBlastSound);
        }
        
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
            updateGame(program, player1, window, FIXED_TIMESTEP, keys);
            //            Update(player1, &program, &window, FIXED_TIMESTEP, fontTexture);
        }
        updateGame(program, player1, window, fixedElapsed, keys);
    }
    
}


void screenSelect(Program* program, GLuint fontTexture, Window* window, Mix_Chunk* laserBlastSound){
    
    switch(GAME_STATE){
        case START_SCREEN:
            program->clearScreen(r,g,b,a);
            DrawText(program->getShaderProgram(), fontTexture, "Play Space Invaders!", 1.0f, 0, -9.5, 1.0f);
            DrawText(program->getShaderProgram(), fontTexture, "(Spacebar to Cont)",  .75f, 0, -6, 0);
            SDL_GL_SwapWindow(window->getDispWindow());
            break;
            
        case GAME_SCREEN:
            playGame(program, window, laserBlastSound);
            GAME_STATE = END_SCREEN;
            break;
            
        case END_SCREEN:
            program->clearScreen(r,g,b,a);
            program->identity();
            DrawText(program->getShaderProgram(), fontTexture, "GAME OVER", 1.0f, 0, 0, 0);
            SDL_GL_SwapWindow(window->getDispWindow());
            break;
    }
}


void playSpaceInvaders(){
//    Int Mix_OpenAudio
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "Space Invaders");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Chunk* laserBlastSound = Mix_LoadWAV("Laser_Blast.wav");
    Mix_Chunk* explosionSound  = Mix_LoadWAV("Explosion.wav");
    
    
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    
    GLuint fontTexture = LoadTexture("font1.png");

    SDL_Event event;
    bool done = false;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        if(keys[SDL_SCANCODE_SPACE] && gameEnded == false ) // create new bullet
            GAME_STATE = GAME_SCREEN;
        
        screenSelect(&program, fontTexture, &window, laserBlastSound);

    }
    Mix_FreeChunk(laserBlastSound);
    SDL_Quit();
    
}

//int main(int argc, char* argv[]){
//    playSpaceInvaders();
//    
//}
*/



























