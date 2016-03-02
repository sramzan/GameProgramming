//
//  space-invaders.cpp
//  
//
//  Created by Sean Ramzan on 2/27/16.
//
//

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Matrix.h"
#include "ShaderProgram.h"

#include "../programGenerator.cpp"
#include "windowGenerator.cpp"

#include "../global-variables.cpp"
#include "sheetSprite.cpp"

#include <vector>

float r = .4f;
float g = .3f;
float b = .5f;
float a = 1.0f;

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


class bullet : public ScreenObject{
public:
    bullet(ShaderProgram* program, GLuint spriteSheetTexture) : ScreenObject(program){
        this->bulletSheetSprite = new SheetSprite(spriteSheetTexture, 845.0f/1024.0f, 0.0f/1024.0f, 13.0f/1024.0f, 57.0f/1024.0f, 1.0);
        setPosition(bulletSheetSprite->getTopPos(), bulletSheetSprite->getBottomPos(), bulletSheetSprite->getLeftPos(), bulletSheetSprite->getRightPos());
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
    
    void checkCollision(){
        
    }
    
    bool isOffScreen(){
        // if (bullet Top pos >= topBoundary of the ortho projection, then delete it from the array)
        return true;
    }
    
private:
    SheetSprite* bulletSheetSprite;
    
};

class player : public ScreenObject{
public:
    player (ShaderProgram* program) : ScreenObject(program){
        this->playerShipSheetSprite = new SheetSprite(this->spriteSheetTexture, 0.0f/1024.0f, 941.0f/1024.0f, 112.0f/1024.0f, 75.0f/1024.0f, 1.0);
        setPosition(playerShipSheetSprite->getTopPos(), playerShipSheetSprite->getBottomPos(), playerShipSheetSprite->getLeftPos(),playerShipSheetSprite->getRightPos());
        setProjection();
        setMatrices();
    }
    
    void fire(){
        bullets.push_back(newBullet());
    }
    
    bullet* newBullet(){
        bullet* newBullet = new bullet(this->program, this->spriteSheetTexture);
        newBullet->setPosition(0, getTopPos() + .5, 0);
        return newBullet;
    }
    
    void moveFiredBulletsUp(float elapsed){
        for (bullet* bullet : bullets){
            bullet->moveUp(7 * elapsed);
            bullet->draw();
        }
    }
    
    void draw(){
        setModelMatrix();
        playerShipSheetSprite->Draw(this);
    }
    
    void destroy(){
//        bullets.erase(remove_if(bullets.begin(), bullets.end(), true), bullets.end());
//        this->destroy();
        for (bullet* bullet : bullets)
            bullet->destroy();
        delete this;
    }
    
    void processMovement(const Uint8* keys, float elapsed){
        if (keys[SDL_SCANCODE_RIGHT])
            moveToTheRight(5 * elapsed);
        if (keys[SDL_SCANCODE_LEFT])
            moveToTheLeft(-5 * elapsed);
    }
    
    
private:
    vector<bullet*> bullets;
    GLuint spriteSheetTexture = LoadTexture("spacePics.png");
    SheetSprite* playerShipSheetSprite;
    
};

void spaceIvadersInit(Program* program){
    
    // Load all textures from texture sheet
    
}

void playSpaceInvaders(){
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "Space Invaders");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    
    /* 
     
     Create()
        - Create all intial objects on the screen
            - aliens
            - spaceship that can shoot
        - In order to create the objects, the proper textures will need to be loaded from the texture xml sheet
     
     Process Actions
        - The bulk of the program will be checking to see what is going on
            1) Did the aliens hit an edge 
                - if so, move them down a row, and reverse the direction
    
            2) Did the player shoot a shot, and did it hit an alien
                - if so, delete the alien from the arr, and continue moving
    */
    
    vector<SheetSprite> sheetSprites;
    vector<ScreenObject> entities;
    float lastFrameTicks = 0.0f;
    
    spaceIvadersInit(&program);
    
//    SheetSprite meteorSprite = SheetSprite(spriteSheetTexture, 224.0f/1024.0f, 664.0f/1024.0f, 101.0f/1024.0f, 84.0f/1024.0f, 1.0);
//    bullet* newBullet = new bullet(program.getShaderProgram(), spriteSheetTexture);
    
    player* player1 = new player(program.getShaderProgram());
    cout << "Bottom Boundary: " << player1->getBottomBoundary() << endl << "Bottom Pos: " << player1->getBottomPos() << endl << "Top Pos: " << player1->getTopPos() << endl << "Top Boundary: " << player1->getTopBoundary();


    player1->moveDown(-fabs(player1->getBottomPos() - player1->getBottomBoundary()));
    
    SDL_Event event;
    bool done = false;
    
    while (!done) {
        
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        float ticks    = (float)SDL_GetTicks()/1000.0f;
        float elapsed  = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        player1->processMovement(keys, elapsed);
        
        program.clearScreen(r,g,b,a);
        player1->draw();
        
        if(keys[SDL_SCANCODE_SPACE]) // create new bullet
            player1->fire();
        
        
        player1->moveFiredBulletsUp(elapsed);
        
        // handle moving all bullets consistently up the screen
        
        
        
        SDL_GL_SwapWindow(window.getDispWindow());
        
    }
    
    player1->destroy();
    SDL_Quit();
    
}

int main(int argc, char* argv[]){
    playSpaceInvaders();

}




























