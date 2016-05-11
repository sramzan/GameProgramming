//
//  platformer-game.cpp
//
//
//  Created by Sean Ramzan on 3/25/16.
//
//

#include "platformer-game.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <unistd.h>
#include "windowGenerator.cpp"
#include "../programGenerator.cpp"
#include "../global-variables.cpp"
using namespace std;

// TODO - Reorganize code to not 'hardcode' two players. Simply keep array of players, and run the same checks/actions for all players (entities) in the array

#define SPRITE_COUNT_X 30 // check this
#define SPRITE_COUNT_Y 30 // check this
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define LEVEL_HEIGHT 30
#define LEVEL_WIDTH 30
#define GRAVITY_X 0
#define GRAVITY_Y -.2
#define FRICTION_X .9
#define FRICTION_Y .99
#define SPRITE_SIZE 21

static ShaderProgram* prog;
static GLuint tileMapSpriteTextureID;
static GLuint entitySpriteTextureID;
float TILE_SIZE=.90f;
float tileMidBaseLine   = 0;
int mapWidth    = -1;
int mapHeight   = -1;

// Level 1 solid pieces
const int solidGround_1 = 191; // TODO - name these more descriptively
const int solidGround_2 = 130;
const int solidGround_3 = 224;
const int solidGround_4 = 194;
const int solidGround_5 = 195;
const int solidGround_6 = 133;

// Level 2 solid pieces
const int underwater_1 = 743;
const int underwater_2 = 332;
const int underwater_3 = 121;
const int underwater_4 = 222;
const int underwater_5 = 123;
const int underwater_6 = 153;
const int underwater_7 = 125;
const int underwater_8 = 151;
const int underwater_9 = 154;
const int underwater_10 = 745;
const int underwater_11 = 776;
const int underwater_12 = 744;
const int underwater_13 = 774;
const int underwater_14 = 746;
const int underwater_15 = 775;
const int underwater_16 = 773;
const int underwater_17 = 122;
const int underwater_18 = 124;


const int iceGround_1 = 607;
const int iceGround_2 = 645;
const int iceGround_3 = 646;
const int iceGround_4 = 225;

const int MENU    = 0;
const int LEVEL_1 = 1;
const int LEVEL_2 = 2;
const int LEVEL_3 = 3;
const int GAME_OVER  = 4;
int GAME_STATE = 0;
bool player1Won = false;

int** levelData;
int** solidTiles;
bool firstRun = true;
int totalArrayHeight = 0;
int totalArrayWidth  = 0;

// bullet sprite sheet info
// player 1
float bullet_uVal   = 0.0f;
float bullet_vVal   = 0.0f;
float bullet_width  = 0.0f;
float bullet_height = 0.0f;

// CONFIGS TO BE CHANGED PER LEVEL OF THE GAME

float scaleViewMatrix_x = 0.0f;
float scaleViewMatrix_y = 0.0f;
float translateViewMatrixOffset_x = 0.0f;
float translateViewMatrixOffset_y = 0.0f;
std::string levelFilePath = "level3.txt";
std::string tileLayerName = "layer";
std::string objLayerName  = "ObjLayer";
Mix_Music* music;

void checkForCollision(Entity* entity, float elapsed, bool isBullet);

GLuint LoadTexture(const char* image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}

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

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

Entity::Entity(ShaderProgram* prog, string entityType, int xPosition, int yPosition, float uVal, float vVal, float widthVal, float heightVal,float sizeVal, int dir, int healthVal) : ScreenObject(prog, entitySpriteTextureID){

    this->type=entityType;
    this->sheetSprite = new SheetSprite(entitySpriteTextureID, uVal, vVal, widthVal, heightVal, sizeVal);
    
    // Set position of entity in space
    this->xPos = (float) (xPosition);
    this->yPos = (float) (yPosition); // *-TILE_SIZE --> ENTITY SHOULD NOT KNOW ABOUT TILED POSITIONS
    
    // default velocity
    this->xVelocity = 0;
    this->yVelocity = 0;
    
    // default acceleration
    this->xAccel = 0;
    this->yAccel = 0;
    
    // setting size of the entity
    this->size = TILE_SIZE;
    
    this->direction = dir;
    this->health = healthVal;
    
    setPositionVars((float)xPosition, (float)yPosition , 0, size);
    
    //    translateViewMatrix(8, -14, 0);
    //    setPosition(x, y, 0, TILE_SIZE);
}

// Entity Class Definitions
string Entity::getType()          { return this->type;      }
int   Entity::getHealth()         { return this->health;    }
int   Entity::getDirection()      { return this->direction; }
float Entity::getX_Acceleration() { return this->xAccel;    }
float Entity::getY_Acceleration() { return this->yAccel;    }
float Entity::getX_Velocity()     { return this->xVelocity; }
float Entity::getY_Velocity()     { return this->yVelocity; }
float Entity::getSize()           { return this->size;      }


float Entity::getXPos() { return this->xPos; }
float Entity::getYPos() { return this->yPos; }

void Entity::applyGravityToVel(float elapsed){
    xVelocity += GRAVITY_X * elapsed;
    yVelocity += GRAVITY_Y * elapsed;
}

void Entity::applyAccelerationToVel(){
    xVelocity += xAccel * FIXED_TIMESTEP;
    yVelocity += yAccel * FIXED_TIMESTEP;
}

void Entity::applyFrictionToVel(){
    xVelocity = lerp(xVelocity, 0.0f, FIXED_TIMESTEP * FRICTION_X); // velX gets a smaller val of itself... some percentage of velX between its val and 0
    yVelocity = lerp(yVelocity, 0.0f, FIXED_TIMESTEP * FRICTION_Y);
}

void Entity::updatePosition(){
    updateXPos(xVelocity);
    updateYPos(yVelocity);
    //    this->xPos += xVelocity * FIXED_TIMESTEP;
    //    this->yPos += yVelocity * FIXED_TIMESTEP;
}

void Entity::updateXPos(float xVel){
    this->xPos += xVel;
}

void Entity::updateYPos(float yVel){
    this->yPos += yVel;
}

void Entity::updateAccelTo(float accelX, float accelY){
    this->xAccel = accelX;
    this->yAccel = accelY;
}

void Entity::setY_Velocity(float vel){
    yVelocity = vel;
}

void Entity::setX_Velocity(float vel){
    xVelocity = vel;
}

void Entity::getVertexAndTextCoordData(){
    float width  = TILE_SIZE/2;
    float height = TILE_SIZE/2;
    
    entityVertexData.insert(entityVertexData.end(), {
        width,  height,
        -width, height,
        -width, -height,
        
        width,   height,
        -width, -height,
        width,   -height
        
    });
    
    entityTextureData.insert(entityTextureData.end(), {
        u, v,
        u, v+(spriteHeight),
        u+spriteWidth, v+(spriteHeight),
        
        u, v,
        u+spriteWidth, v+(spriteHeight),
        u+spriteWidth, v
    });
}

void Entity::draw(){
    drawTexture(entityVertexData.data(), entityTextureData.data());
}

void Entity::applyAccelerationToVelFor(Bullet* bullet){
    bullet->applyAccelerationToVel();
}

void Entity::fire(Mix_Chunk* energyBlastSound){
    Bullet* newBullet = NULL;
    if (direction == 1)
        newBullet = new Bullet(program, xPos + TILE_SIZE, yPos - TILE_SIZE/2 , direction);
    if (direction == -1)
        newBullet = new Bullet(program, xPos - TILE_SIZE/2, yPos - TILE_SIZE/2 , direction);
    Mix_PlayChannel( -1, energyBlastSound, 0);
//    newBullet->updateAccelTo(.2 * direction, 0);
//    newBullet->move(.2* direction, 0, 0);
    newBullet->updateAccelTo(5 * direction, 0);
    applyAccelerationToVelFor(newBullet);
    bullets.push_back(newBullet);
}

void Entity::setDirectionLeft(){
    direction = -1;
}

void Entity::setDirectionRight(){
    direction = 1;
}

void Entity::moveBullets(){
    if (bullets.size() == 0)
        return;
    for (Bullet* bullet : bullets){
        if (bullet->getBulletDirection() == 1){
//            bullet->moveToTheRight(.2);
            bullet->updateAccelTo(.2, 0);
        }
        if (bullet->getBulletDirection() == -1){
//            bullet->moveToTheLeft(.2);
            bullet->updateAccelTo(-.2, 0);
        }
    }
}

void Entity::checkBulletCollisions(float elapsed){
    for (int index = 0; index < bullets.size(); index++){
        Bullet* bullet = bullets[index];
        checkForCollision(bullet, elapsed, true);
        if (bullet->isMarkedForDeletion()){
            bullets.erase(bullets.begin() + index);
            delete bullet;
//            delete bullets[index];
        }
    }
}

void Entity::checkBulletCollisions(Entity* player, float elapsed){
    float playerSize   = player->getSize();
    float entityLeft   = player->getXPos() - (playerSize/2);
    float entityRight  = player->getXPos();// + (playerSize/2);
    float entityTop    = player->getYPos() + (playerSize/2);
    float entityBottom = player->getYPos() - (playerSize/2);
    
    for (int index = 0; index < bullets.size(); index++){
        Bullet* bullet   = bullets[index];
        float bulletSize = bullet->getSize();
        int bulletLeft   = bullet->getXPos(); //- (bulletSize/2);
        int bulletRight  = bullet->getXPos() + (bulletSize/2);
        int bulletTop    = bullet->getYPos() + (bulletSize/2);
        int bulletBottom = bullet->getYPos() - (bulletSize/2);
        
        //check top bullet & bottom entity
        if (bullet->getDirection() == 1 && (player->getXPos() >= bullet->getInitialX())){
            if (  ((bulletTop >= entityBottom) && (bulletRight >= entityLeft)) && ((bulletBottom <= entityTop) && (bulletRight >= entityLeft))){
                player->decrementHealth();
                cout << "Player " + player->getType() + " was hit... Current Health: " << player->getHealth() << endl;
                bullets.erase(bullets.begin() + index);
                delete bullet;
            }
        }
        
        if ((bullet->getDirection() == -1) && (player->getXPos() <= bullet->getInitialX())){
            
            if(  ((bulletTop >= entityBottom) && (bulletLeft  <= entityRight)) && ((bulletBottom <= entityTop) && (bulletLeft <= entityRight))){
                player->decrementHealth();
                cout << "Player " + player->getType() + " was hit... Current Health: " << player->getHealth() << endl;
                bullets.erase(bullets.begin() + index);
                delete bullet;
            }
        }
        
            // TODO - switch logic here to just mark for deletion and let the other collision check function take care of the deleting
        }
}


void Entity::drawBullets(){
    if (bullets.size() > 0){
        for (Bullet* bullet : bullets){
            program->setModelMatrix(bullet->getModelMatrix());
            bullet->drawFromSheetSprite();
        }
    }
    
}

void Entity::setBulletsToIdentity(){
    if (bullets.size() > 0){
        for (Bullet* bullet : bullets)
            bullet->identity();
    }
}

void Entity::moveAllBulletsToPosition(){
    if (bullets.size() > 0 ){
        for (Bullet* bullet : bullets)
            bullet->move(bullet->getXPos(), bullet->getYPos(), 0);
    }
}

void Entity::applyAccelerationToVelForAllBullets(){
    if (bullets.size() > 0){
        for (Bullet* bullet : bullets)
            bullet->applyAccelerationToVel();
    }
}

void Entity::updateAllBulletPositions(){
    if (bullets.size() > 0){
        for (Bullet* bullet : bullets)
            bullet->updatePosition();
    }
}

void Entity::invertBulletsIfNecessary(){
    if (bullets.size() > 0){
        for (Bullet * bullet : bullets){
            if (bullet->getBulletDirection() == -1)
                bullet->scale(-1, 1, 1);
        }
            
    }
}

void Entity::decrementHealth(){
    health--;
}

void Entity::deleteAllBullets(){
    for (int index = 0; index < bullets.size(); index++){
        bullets.erase(bullets.begin() + index);
        delete bullets[index];
    }
    bullets.clear();
}

void Entity::resetHealth(){
    health = 5;
}

Bullet::Bullet(ShaderProgram* prog, float xPos, float yPos, int dir) : Entity(prog, "bullet", xPos, yPos, bullet_uVal/512.0f, bullet_vVal/512.0f, bullet_width/512.0f, bullet_height/512.0f, TILE_SIZE/2, dir, 0){ // bullets have a natural health of 0
    this->bulletDirection = dir;
    this->markedForDeletion = false;
    this->initialXPos = xPos;
    this->initialYPos = yPos;
    
}
void Bullet::markForDeletion(){
    this->markedForDeletion = true;
}

bool Bullet::isMarkedForDeletion(){
    return markedForDeletion;
}

int Bullet::getBulletDirection(){
    return bulletDirection;
}

float Bullet::getInitialX(){
    return initialXPos;
}

float Bullet::getInitialY(){
    return initialYPos;
}



// TileMap Class Definitions
TileMap::TileMap(ShaderProgram* prog) : ScreenObject(prog, tileMapSpriteTextureID){}
vector<float>* TileMap::getVertexVector() { return &vertexData;   }
vector<float>* TileMap::getTextVector()   { return &texCoordData; }
float* TileMap::getVertexData() { return vertexData.data();   }
float* TileMap::getTextData()   { return texCoordData.data(); }

void drawTexture(float* verticesArray, float* texCords){
    //        if (texture == NULL)
    //            throw new LoadException("Texture not defined for " + textureName + "\n");
    
    glEnable(GL_BLEND);
    //        glBindTexture(GL_TEXTURE_2D, texture); // whatever texture is bound, is the one that will be mapped to the textureCoordinates
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glVertexAttribPointer(prog->positionAttribute, 2, GL_FLOAT, false, 0, verticesArray);
    glEnableVertexAttribArray(prog->positionAttribute);
    
    glVertexAttribPointer(prog->texCoordAttribute, 2, GL_FLOAT, false, 0, texCords);
    glEnableVertexAttribArray(prog->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, totalArrayHeight * totalArrayWidth * 6); //LEVEL_HEIGHT * LEVEL_WIDTH * 6);
    
    glDisableVertexAttribArray(prog->positionAttribute);
    glDisableVertexAttribArray(prog->texCoordAttribute);
}

int getSpriteSheetValof(Entity* entity){
    static playerMapping players;
    players["pinkPlayer"] = 57;
    return players[entity->getType()];
}

bool isSolidGround(int val){
    return (val == solidGround_1) ? true : false;
}

bool isSolidTile(int val){
    switch (val){
        case solidGround_1:
            return true;
            break;
        case solidGround_2:
            return true;
            break;
        case solidGround_3:
            return true;
            break;
        case solidGround_4:
            return true;
            break;
        case solidGround_5:
            return true;
            break;
        case solidGround_6:
            return true;
            break;

        case underwater_1:
            return true;
            break;
        case underwater_2:
            return true;
            break;
        case underwater_3:
            return true;
            break;
        case underwater_4:
            return true;
            break;
        case underwater_5:
            return true;
            break;
        case underwater_6:
            return true;
            break;
        case underwater_7:
            return true;
            break;
        case underwater_8:
            return true;
            break;
        case underwater_9:
            return true;
            break;
        case underwater_10:
            return true;
            break;
        case underwater_11:
            return true;
            break;
        case underwater_12:
            return true;
            break;
        case underwater_13:
            return true;
            break;
        case underwater_14:
            return true;
            break;
        case underwater_15:
            return true;
            break;
        case underwater_16:
            return true;
            break;
        case underwater_17:
            return true;
            break;
        case underwater_18:
            return true;
            break;
            
        case iceGround_1:
            return true;
            break;
        case iceGround_2:
            return true;
            break;
        case iceGround_3:
            return true;
            break;
        case iceGround_4:
            return true;
            break;
            
        // for level3
    }
    
    return false;
}

void getVertexAndTextureCoordsFromTileMap(TileMap* tm){
    vector<float>* tilemapVertexData = tm->getVertexVector();
    vector<float>* tilemapTextData   = tm->getTextVector();
    for(int y=0; y < LEVEL_HEIGHT; y++) {
        for(int x=0; x < LEVEL_WIDTH; x++) {
            if(levelData[y][x] != 0){
//                if (isSolidGround(levelData[y][x]) && tileMidBaseLine == 0){
//                    tileMidBaseLine = y - (TILE_SIZE / 2);
//                }
                float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
                float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
                float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
                float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
                tilemapVertexData->insert(tilemapVertexData->end(), {
                    TILE_SIZE * x,              -TILE_SIZE * y,
                    TILE_SIZE * x,             (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE,  (-TILE_SIZE * y)-TILE_SIZE,
                    TILE_SIZE * x,              -TILE_SIZE * y,
                    (TILE_SIZE * x)+TILE_SIZE,  (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE,   -TILE_SIZE * y
                });
                tilemapTextData->insert(tilemapTextData->end(), {
                    u, v,
                    u, v+(spriteHeight),
                    u+spriteWidth, v+(spriteHeight),
                    u, v,
                    u+spriteWidth, v+(spriteHeight),
                    u+spriteWidth, v
                });
            }
            totalArrayWidth = x;
        }
        totalArrayHeight = y;
    }
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) { // way to get entities' grid location in space
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}

void placeEntity(string type, float placeX, float placeY){

    // <SubTexture name="slice29_29.png" x="207" y="0" width="32" height="43"/> - Standard Vegito
    // <SubTexture name="planeBlue1.png" x="0" y="73" width="88" height="73"/>  - Super Saiyan Vegito
    float x, y, width, height;
    int direction, health=5; // every player starts off with a health level of 3
    
    if (type=="player1"){
        x = 207.0f;
        y = 0.0f;
        width = 32.0f;
        height = 43.0f;
        direction = 1;
    }
//    <SubTexture name="slice136_@.png" x="233" y="133" width="32" height="43"/>
    if (type == "player2"){
        x = 233.0f;
        y = 133.0f;
        width = 32.0f;
        height = 43.0f;
        direction = -1;
    }
    
    Entity* newEntity = new Entity(prog, type.c_str(), placeX, placeY, x/512.0f, y/512.0f, width/512.0f, height/512.0f, TILE_SIZE, direction, health);
    entities.push_back(newEntity);
}

bool readHeader(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        } else if(key == "height"){
            mapHeight = atoi(value.c_str());
        }
    }
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    } else { // allocate our map data
        levelData = new int*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new int[mapWidth];
        }
        return true;
    }
    
    return true;
}

bool readLayerData(ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
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
                    int val = atoi(tile.c_str());
                    if(val > 0) {
                        
                        // be careful, the tiles in this format are indexed from 1 not 0
                        //                        if (val - 1 == solidGround)
                        //                        if (val == solidGround)
                        //                            solidTiles[y][x]
                        cout << "y: " << y << " x: " << x << " val: " << val-1 << endl;
                        levelData[y][x] = val-1;
                    } else {
                        cout << "y: " << y << " x: " << x << " val: " << 0 << endl;
                        levelData[y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

bool readEntityData(ifstream &stream) {
    string line;
    string type;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "type") {
            type = value;
        } else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            //            float placeX = atoi(xPosition.c_str()) / 21*TILE_SIZE; // NOT GETTING PROPER INDEX... FIXME
            //            float placeY = atoi(yPosition.c_str()) / 21*-TILE_SIZE;
            float placeX = atoi(xPosition.c_str())  * TILE_SIZE; //atoi(xPosition.c_str()); // NOT GETTING PROPER INDEX... FIXME
            float placeY = atoi(yPosition.c_str()) * -TILE_SIZE; //atoi(yPosition.c_str());
            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}

void readFileLineByLine(){
    ifstream infile(levelFilePath);
    string line;
    while (getline(infile, line)) {
        if(line == "[header]") {
            if(!readHeader(infile)) {
                return;
            }
        } else if(line == "["+tileLayerName+"]") {
            readLayerData(infile);
        } else if(line == "["+objLayerName+"]") {
            readEntityData(infile);
        }
    }
    //    infile.close();
}

void checkForCollision(Entity* entity, float elapsed, bool isBullet){
    int xLoc, yLoc;
    float entityTop    = entity->getYPos() + TILE_SIZE/2.0;
    float entityBottom = entity->getYPos() - TILE_SIZE/2.0;
    float entityLeft   = entity->getXPos() - TILE_SIZE/2.0  ;
    float entityRight  = entity->getXPos() + TILE_SIZE/2.0;
    
    
    //check bottom
    worldToTileCoordinates(entity->getXPos(), entityBottom, &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
//        cout << "BOTTOM COLLISION" << endl;
        if (isBullet){
//            entity->deleteBullet((Bullet*)entity); // may cause an issue
//            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs((yLoc*-TILE_SIZE) - entityBottom);
            entity->moveUp(penetration + .0001);
            entity->updateYPos(penetration + .0001);
            //.00000001
        }
    }
    
    // checking top collision
    worldToTileCoordinates(entity->getXPos(), entityTop, &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
//        cout << "TOP COLLISION" << endl;
        if (isBullet){
//            ((Bullet*) entity)->markForDeletion();
            
        }else{
            float y_distance = fabs(((yLoc*-TILE_SIZE) - TILE_SIZE) - entityTop);
            float penetration = y_distance;
            entity->moveDown(-penetration + .0001);
            entity->updateYPos(-penetration + .0001);
        }
        //        entity->setY_Velocity(0);
    }
    
    // checking left collision
    worldToTileCoordinates(entityLeft, entity->getYPos(), &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
//        cout << "Left COLLISION" << endl;
        if (isBullet){
            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs(((TILE_SIZE * xLoc) + TILE_SIZE) - entityLeft);
            entity->moveToTheRight(penetration + .0001);
            entity->updateXPos(penetration + .0001);
        }
        //        entity->setY_Velocity(0);
    }

    // checking right collision
    worldToTileCoordinates(entityRight, entity->getYPos(), &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
//        cout << "RIGHT COLLISION" << endl;
        if (isBullet){
            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs((TILE_SIZE * xLoc) - entityRight);
            entity->moveToTheLeft(-penetration + .0001);
            entity->updateXPos(-penetration + .0001);
        }
        //        entity->setY_Velocity(0);
    }
    
    
}

void handleInputCommands(Entity* player1, Entity* player2, const Uint8* keys){
    // Player 1 Commands
    if (keys[SDL_SCANCODE_RIGHT]){
        player1->updateAccelTo(.1, 0);
        player1->applyAccelerationToVel();
        //            entity->addToX(entity->getX_Acceleration());
    }
    if (keys[SDL_SCANCODE_LEFT]){
        player1->updateAccelTo(-.1, 0);
        player1->applyAccelerationToVel();
    }
    
    if (keys[SDL_SCANCODE_DOWN]){
        player1->updateAccelTo(0, -.1);
        player1->applyAccelerationToVel();
    }
    
    if (keys[SDL_SCANCODE_UP]){
        player1->updateAccelTo(0, .2);
        player1->applyAccelerationToVel();
    }
    
    
    // Player 2 Commands
    if (keys[SDL_SCANCODE_D]){
        player2->updateAccelTo(.1, 0);
        player2->applyAccelerationToVel();
        //            entity->addToX(entity->getX_Acceleration());
    }
    if (keys[SDL_SCANCODE_A]){
        player2->updateAccelTo(-.1, 0);
        player2->applyAccelerationToVel();
    }
    
    if (keys[SDL_SCANCODE_S]){
        player2->updateAccelTo(0, -.1);
        player2->applyAccelerationToVel();
    }
    
    if (keys[SDL_SCANCODE_W]){
        player2->updateAccelTo(0, .2);
        player2->applyAccelerationToVel();
    }
}

void checkIfPlayerBulletsHitOtherPlayer(Entity* player1, Entity* player2, float elapsed){
    player1->checkBulletCollisions(player2, elapsed);
}

void handleEntityActions(Entity* player1, Entity* player2, const Uint8* keys, float elapsed, Program* program){
    
    if (player1 == NULL || player2 == NULL)
        return;
    
    
    player1->identity();
    player1->setBulletsToIdentity();
    player2->identity();
    player2->setBulletsToIdentity();
    
    player1->moveAllBulletsToPosition();
    player1->move(player1->getXPos(), player1->getYPos(), 0);
    player2->moveAllBulletsToPosition();
    player2->move(player2->getXPos(), player2->getYPos(), 0);


    handleInputCommands(player1, player2, keys);
    
//    applyPhysicsTo(player1); // TODO make code more readable by implementing these functions
//    applyPhysicsTo(player2);

    player1->applyFrictionToVel();
    player1->applyGravityToVel(elapsed);
    player1->move(player1->getX_Velocity(), player1->getY_Velocity(), 0);
    
    
    player1->checkBulletCollisions(elapsed);
//    player1->moveBullets();
    player1->updateAllBulletPositions();
    checkIfPlayerBulletsHitOtherPlayer(player1, player2, elapsed);
    checkForCollision(player1, elapsed, false);
    player1->updatePosition();
    player1->updateAccelTo(0, 0);
    
    player2->applyFrictionToVel();
    player2->applyGravityToVel(elapsed);
    player2->move(player1->getX_Velocity(), player1->getY_Velocity(), 0);
    player2->checkBulletCollisions(elapsed);
    player2->updateAllBulletPositions();
    checkIfPlayerBulletsHitOtherPlayer(player2, player1, elapsed);
    checkForCollision(player2, elapsed, false);
    player2->updatePosition();
    player2->updateAccelTo(0, 0);
    

    if (player1->getX_Velocity() < 0){
        player1->scale(-1, 1, 1);
        player1->setDirectionLeft();
    }else{
        player1->setDirectionRight();
    }
    
    player1->invertBulletsIfNecessary();
    
    
    if (player2->getX_Velocity() < 0){
        player2->scale(-1, 1, 1);
        player2->setDirectionLeft();
    }else{
        player2->setDirectionRight();
    }
    
    player2->invertBulletsIfNecessary();
    
//    entity->scale(-1, 1 ,1);
    
//    entity->draw();
//    entity->drawFromSheetSprite();
//    program->translateViewMatrix(-1 * entity->getXPos(), -1 * entity->getYPos(), 0);
}

void playGame(Program* program, SDL_Event* event, Window* window){
    program->resetMatrices();
    
    tileMapSpriteTextureID = LoadTexture("spriteSheet.png");
    //    tileMapSpriteTextureID = LoadTexture("sprites");
    entitySpriteTextureID  = LoadTexture("DBZ_sprites.png");
    
    bool done            = false;
    float lastFrameTicks = 0.0f;
    bool getEntity       = true;
    
    TileMap* tileMap  = new TileMap(prog);
    Entity* player1   = NULL;
    Entity* player2   = NULL;
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Chunk* player1Sound  = Mix_LoadWAV("energyBlast1.wav");
    Mix_Chunk* player2Sound  = Mix_LoadWAV("energyBlast2.wav");
    Mix_PlayMusic(music, -1);
    
    // DO THIS AFTER CHOOSING A LEVEL
    readFileLineByLine();
    getVertexAndTextureCoordsFromTileMap(tileMap);
    if (entities.size() > 0){
        if(getEntity){
            player1 = entities[0];
            player2 = entities[1];
            getEntity = false;
        }
    }
    
    program->setViewMatrixToIdentity();
//    program->scaleViewMatrix(.8, .8, 0);
    program->scaleViewMatrix(scaleViewMatrix_x, scaleViewMatrix_y, 0);

    program->translateViewMatrix(tileMap->getLeftBoundary() + translateViewMatrixOffset_x, tileMap->getTopBoundary() + translateViewMatrixOffset_y, 0);
    program->setViewMatrix();
    //    program.translateViewMatrix(tileMap->getLeftBoundary() * 1.10, tileMap->getTopBoundary() * 2, 0); WITH TILE SIZE AS 1.25
    
    
    
    while (!done) {
        float ticks    = (float)SDL_GetTicks()/1000.0f;
        float elapsed  = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        while (SDL_PollEvent(event)) {
            if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            
            if (event->type == SDL_KEYDOWN){
                if (event->key.keysym.scancode == SDL_SCANCODE_SPACE){
                    //<SubTexture name="slice103_@.png" x="0" y="179" width="63" height="56"/>
                    bullet_uVal   = 0.0f;
                    bullet_vVal   = 179.0f;
                    bullet_width  = 63.0f;
                    bullet_height = 56.0f;
                    player1->fire(player1Sound);
                }
                
                if (event->key.keysym.scancode == SDL_SCANCODE_Z){
                    //    <SubTexture name="slice139_@.png" x="178" y="148" width="16" height="12"/> Bullet location on sprite sheet
                    bullet_uVal   = 178.0f;
                    bullet_vVal   = 148.0f;
                    bullet_width  = 16.0f;
                    bullet_height = 12.0f;
                    player2->fire(player2Sound);
                }
                
                if (event->key.keysym.scancode == SDL_SCANCODE_M){
                    GAME_STATE = MENU;
                    player1->deleteAllBullets();
                    player2->deleteAllBullets();
                    for (int index = 0; index < entities.size(); index++){
                        entities.erase(entities.begin() + index);
                        delete entities[index];
                    }
                    entities.clear();
                    Mix_FreeChunk(player1Sound);
                    Mix_FreeChunk(player2Sound);
                    Mix_FreeMusic(music);
                    return;
                }
                
            }
            
            
        }
        
        program->clearScreen(0, 0, 0, 0);
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        tileMap->identity();
        program->setModelMatrix(tileMap->getModelMatrix());
        glBindTexture(GL_TEXTURE_2D, tileMapSpriteTextureID);
        drawTexture(tileMap->getVertexData(), tileMap->getTextData());
        
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        
        glBindTexture(GL_TEXTURE_2D, entitySpriteTextureID);
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
            handleEntityActions(player1, player2, keys, fixedElapsed, program);
            
        }
        handleEntityActions(player1, player2, keys, fixedElapsed, program);
        getEntity = false;
        
        
        program->setModelMatrix(player1->getModelMatrix());
        glBindTexture(GL_TEXTURE_2D, entitySpriteTextureID);
        player1->drawFromSheetSprite();
        player1->drawBullets();
        program->setModelMatrix(player2->getModelMatrix());
        player2->drawFromSheetSprite();
        player2->drawBullets();
        
        if (player2->getHealth() <= 0){
            GAME_STATE = GAME_OVER;
            player1->deleteAllBullets();
            player2->deleteAllBullets();
            for (int index = 0; index < entities.size(); index++){
                entities.erase(entities.begin() + index);
                delete entities[index];
            }
            entities.clear();
            player1Won = true;
            Mix_FreeChunk(player1Sound);
            Mix_FreeChunk(player2Sound);
            Mix_FreeMusic(music);
            return;
        }
        
        if (player1->getHealth() <= 0){
            GAME_STATE = GAME_OVER;
            player1Won = false;
            player1->deleteAllBullets();
            player2->deleteAllBullets();
            for (int index = 0; index < entities.size(); index++){
                entities.erase(entities.begin() + index);
                delete entities[index];
            }
            entities.clear();
            Mix_FreeChunk(player1Sound);
            Mix_FreeChunk(player2Sound);
            Mix_FreeMusic(music);
            return;
        }
        
        SDL_GL_SwapWindow(window->getDispWindow());
        
    }
    Mix_FreeChunk(player1Sound);
    Mix_FreeChunk(player2Sound);
    SDL_Quit();
    
}

void determineGameState(Program* program, GLuint fontTexture, Window* window, SDL_Event* event){
    switch (GAME_STATE){
        case MENU:
            //display the menu and options
            program->resetMatrices();
            program->clearScreen(0,0,0,0);
            DrawText(program->getShaderProgram(), fontTexture, "Choose Level!",  1.50f, 0, -8.5, 5);
            DrawText(program->getShaderProgram(), fontTexture, "Level 1",    1.00f, 0, -3, 3);
            DrawText(program->getShaderProgram(), fontTexture, "Level 2",    1.00f, 0, -3, 1.5);
            DrawText(program->getShaderProgram(), fontTexture, "Level 3",    1.00f, 0, -3, 0);
            DrawText(program->getShaderProgram(), fontTexture, "Quit Game",  1.00f, 0, -4, -1.5);
            SDL_GL_SwapWindow(window->getDispWindow());
//            GAME_STATE = displayAndGetMenuChoice();
            break;
        case LEVEL_1:
            cout << "Loading configs for Level 1";
            levelFilePath = "level1.txt";
            scaleViewMatrix_x = 0.65f;
            scaleViewMatrix_y = 0.65f;
            translateViewMatrixOffset_x = 1.0f;
            translateViewMatrixOffset_y = 4.5f;
//            Mix_FreeMusic(music);
            music = Mix_LoadMUS("happy_ukulele.mp3");
            playGame(program, event, window);
            break;
            
        case LEVEL_2:
            cout << "Loading configs for Level 2";
            levelFilePath = "level2.txt";
            scaleViewMatrix_x = 0.65f;
            scaleViewMatrix_y = 0.65f;
            translateViewMatrixOffset_x = 1.0f;
            translateViewMatrixOffset_y = 4.5f;
            //            Mix_FreeMusic(music);
            music = Mix_LoadMUS("under_the_sea.mp3");
            playGame(program, event, window);
            break;
            
        case LEVEL_3:
            cout << "Loading configs for Level 3";
            levelFilePath = "level3.txt";
            scaleViewMatrix_x = 0.8f;
            scaleViewMatrix_y = 0.8f;
            translateViewMatrixOffset_x = 1.0f;
            translateViewMatrixOffset_y = 4.5f;
//            Mix_FreeMusic(music);
            music = Mix_LoadMUS("background_music_level1.mp3");
            playGame(program, event, window);
            break;
        case GAME_OVER:
            music = Mix_LoadMUS("background_music_level1.mp3");
            Mix_PlayMusic(music, -1);
            bool timeForMainMenu = false;
            //display the menu and options
            while((!timeForMainMenu)){
                  while(SDL_PollEvent(event)){
                      if (event->key.keysym.scancode == SDL_SCANCODE_M){
                          timeForMainMenu=true;
                          break;
                      }
                program->resetMatrices();
                program->clearScreen(0,0,0,0);
                DrawText(program->getShaderProgram(), fontTexture, "Game Over",  1.50f, 0, -5.5, 5);
                string playerThatWon = (player1Won == true) ? "Player 1" : "Player 2";
                DrawText(program->getShaderProgram(), fontTexture, playerThatWon + " is victorious!",  1.00f, 0, -11, 2);
                SDL_GL_SwapWindow(window->getDispWindow());
            //            GAME_STATE = displayAndGetMenuChoice();
                  }
            }
            Mix_FreeMusic(music);
            GAME_STATE = MENU;
            break;
    }
    
}


void playSpaceInvaders(){
    //    Int Mix_OpenAudio
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "The Awesome Platform Game - SR");
    
#ifdef _WINDOWS
    glewInit();
#endif
    

    GLuint fontTexture = LoadTexture("font1.png");
    
    
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    prog = program.getShaderProgram();
    
    SDL_Event event;
    
    tileMapSpriteTextureID = LoadTexture("spriteSheet.png");
    //    tileMapSpriteTextureID = LoadTexture("sprites");
    entitySpriteTextureID  = LoadTexture("DBZ_sprites.png");
    
    bool done            = false;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            
            if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.scancode == SDL_SCANCODE_1){
                    GAME_STATE = LEVEL_1;
                }
                
                if (event.key.keysym.scancode == SDL_SCANCODE_2){
                    GAME_STATE = LEVEL_2;
                }
                
                if (event.key.keysym.scancode == SDL_SCANCODE_3){
                   GAME_STATE = LEVEL_3;
                    
                }
                
                if (event.key.keysym.scancode == SDL_SCANCODE_Q){
                    done = true;
                    
                }
            }

        }
        
//        if(keys[SDL_SCANCODE_SPACE] && gameEnded == false ) // create new bullet
//            GAME_STATE = MENU;
        
        determineGameState(&program, fontTexture, &window, &event);
        
    }
    SDL_Quit();
    
}


int main(int argc, char* argv[]){
    char * dir = getcwd(NULL, 0);
    printf("Current dir: %s", dir);
    playSpaceInvaders();
    
//    playPlatformGame();
    
//    bullet
//    <SubTexture name="slice139_@.png" x="178" y="148" width="16" height="12"/>
    
}