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
#define FRICTION_X .7
#define FRICTION_Y .99
#define SPRITE_SIZE 21

static ShaderProgram* prog;
static GLuint tileMapSpriteTextureID;
static GLuint entitySpriteTextureID;
float TILE_SIZE=.90f;
float tileMidBaseLine   = 0;
int mapWidth    = -1;
int mapHeight   = -1;
const int solidGround_1 = 586; // TODO - name these more descriptively
const int solidGround_2 = 555;
const int solidGround_3 = 557;

const int iceGround_1 = 607;
const int iceGround_2 = 645;
const int iceGround_3 = 646;
const int iceGround_4 = 191;

int** levelData;
int** solidTiles;
bool firstRun = true;
int totalArrayHeight = 0;
int totalArrayWidth  = 0;

std::string levelFilePath = "level3.txt";
std::string tileLayerName = "layer";
std::string objLayerName  = "ObjLayer";

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
int   Entity::getDirection()      { return this->direction; }
float Entity::getX_Acceleration() { return this->xAccel;    }
float Entity::getY_Acceleration() { return this->yAccel;    }
float Entity::getX_Velocity()     { return this->xVelocity; }
float Entity::getY_Velocity()     { return this->yVelocity; }

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

void Entity::fire(){
    Bullet* newBullet = NULL;
    if (direction == 1)
        newBullet = new Bullet(program, xPos + TILE_SIZE, yPos - TILE_SIZE/2 , direction);
    if (direction == -1)
        newBullet = new Bullet(program, xPos - TILE_SIZE/2, yPos - TILE_SIZE/2 , direction);
        
//    newBullet->updateAccelTo(.2 * direction, 0);
//    newBullet->move(.2* direction, 0, 0);
    newBullet->updateAccelTo(3 * direction, 0);
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

Bullet::Bullet(ShaderProgram* prog, int xPos, int yPos, int dir) : Entity(prog, "bullet", xPos, yPos, 178.0f/512.0f, 148.0f/512.0f, 16/512.0f, 12.0f/512.0f, TILE_SIZE/2, dir){
    this->bulletDirection = dir;
    this->markedForDeletion = false;
    
//    <SubTexture name="slice139_@.png" x="178" y="148" width="16" height="12"/> Bullet location on sprite sheet
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
    int direction, health=3; // every player starts off with a health level of 3
    
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
        cout << "BOTTOM COLLISION" << endl;
        if (isBullet){
//            entity->deleteBullet((Bullet*)entity); // may cause an issue
//            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs((yLoc*-TILE_SIZE) - entityBottom);
            entity->moveUp(penetration + .00000001);
            entity->updateYPos(penetration + .00000001);
        }
    }
    
    // checking top collision
    worldToTileCoordinates(entity->getXPos(), entityTop, &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
        cout << "TOP COLLISION" << endl;
        if (isBullet){
//            ((Bullet*) entity)->markForDeletion();
            
        }else{
            float y_distance = fabs(((yLoc*-TILE_SIZE) - TILE_SIZE) - entityTop);
            float penetration = y_distance;
            entity->moveDown(-penetration + .00000001);
            entity->updateYPos(-penetration + .00000001);
        }
        //        entity->setY_Velocity(0);
    }
    
    // checking left collision
    worldToTileCoordinates(entityLeft, entity->getYPos(), &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
        cout << "Left COLLISION" << endl;
        if (isBullet){
            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs(((TILE_SIZE * xLoc) + TILE_SIZE) - entityLeft);
            entity->moveToTheRight(penetration + .00000001);
            entity->updateXPos(penetration + .00000001);
        }
        //        entity->setY_Velocity(0);
    }

    // checking right collision
    worldToTileCoordinates(entityRight, entity->getYPos(), &xLoc, &yLoc);
    if (isSolidTile(levelData[yLoc][xLoc])){
        cout << "RIGHT COLLISION" << endl;
        if (isBullet){
            ((Bullet*) entity)->markForDeletion();
        }else{
            float penetration = fabs((TILE_SIZE * xLoc) - entityRight);
            entity->moveToTheLeft(-penetration + .00000001);
            entity->updateXPos(-penetration + .00000001);
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

void checkIfPlayerBulletsHitOtherPlayer(Entity* player1, Entity* player2){
    
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
    
    checkForCollision(player1, elapsed, false);
    player1->updatePosition();
    player1->updateAccelTo(0, 0);
    
    player2->applyFrictionToVel();
    player2->applyGravityToVel(elapsed);
    player2->move(player1->getX_Velocity(), player1->getY_Velocity(), 0);
    player2->checkBulletCollisions(elapsed);
    player2->updateAllBulletPositions();
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
        player2awwww->setDirectionRight();
    }
    
    player2->invertBulletsIfNecessary();
    
//    entity->scale(-1, 1 ,1);
    
//    entity->draw();
//    entity->drawFromSheetSprite();
//    program->translateViewMatrix(-1 * entity->getXPos(), -1 * entity->getYPos(), 0);
}

void playPlatformGame(){
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "The Awesome Platform Game - SR");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    prog = program.getShaderProgram();
    
    SDL_Event event;
    
    tileMapSpriteTextureID = LoadTexture("spriteSheet.png");
//    tileMapSpriteTextureID = LoadTexture("sprites");
    entitySpriteTextureID  = LoadTexture("DBZ_sprites.png");
    
    bool done            = false;
    float lastFrameTicks = 0.0f;
    bool getEntity       = true;
    
    TileMap* tileMap = new TileMap(prog);
    Entity* player1   = NULL;
    Entity* player2   = NULL;
    readFileLineByLine();
    getVertexAndTextureCoordsFromTileMap(tileMap);
    if (entities.size() > 0){
        if(getEntity){
            player1 = entities[0];
            player2 = entities[1];
            getEntity = false;
        }
    }
    
//    program.scaleViewMatrix(1.5, 1.5, 0);
//    tileMap->identity();
    program.setViewMatrixToIdentity();
    program.scaleViewMatrix(.8, .8, 0);

    program.translateViewMatrix(tileMap->getLeftBoundary() + 1, tileMap->getTopBoundary() + 4.5, 0);
    //    program.translateViewMatrix(tileMap->getLeftBoundary() + 1, tileMap->getTopBoundary() + 4, 0);
    program.setViewMatrix();
//    program.translateViewMatrix(tileMap->getLeftBoundary() * 1.10, tileMap->getTopBoundary() * 2, 0); WITH TILE SIZE AS 1.25
    
    
    
    while (!done) {
        float ticks    = (float)SDL_GetTicks()/1000.0f;
        float elapsed  = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            
            if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                    player1->fire();
                }
                
                if (event.key.keysym.scancode == SDL_SCANCODE_Z){
                    player2->fire();
                }
            }
            
            
        }
        
//        program.clearScreen(.5, .3, .4, 1);
        program.clearScreen(0, 0, 0, 0);
        
//        if (!getEntity)
//            program.translateViewMatrix(-entity->getXPos(), -entity->getYPos(), 0);
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        tileMap->identity();
        program.setModelMatrix(tileMap->getModelMatrix());
        glBindTexture(GL_TEXTURE_2D, tileMapSpriteTextureID);
        drawTexture(tileMap->getVertexData(), tileMap->getTextData());

//        if (!getEntity){
//            program.setModelMatrix(entity->getModelMatrix());
//            glBindTexture(GL_TEXTURE_2D, entitySpriteTextureID);
//            entity->drawFromSheetSprite();
//        }
        
//        glBindTexture(GL_TEXTURE_2D, spriteText);
        /*
        if (keys[SDL_SCANCODE_SPACE]){
            // add bullet to player/fire bullet
            player1->fire();
            //        player1->applyAccelerationToVelForAllBullets();
        }
         */
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        
            glBindTexture(GL_TEXTURE_2D, entitySpriteTextureID);
            while (fixedElapsed >= FIXED_TIMESTEP ) {
                fixedElapsed -= FIXED_TIMESTEP;
                //              updateGame(program, player1, window, FIXED_TIMESTEP, keys);
                handleEntityActions(player1, player2, keys, fixedElapsed, &program);
                
            }
            handleEntityActions(player1, player2, keys, fixedElapsed, &program);
            getEntity = false;
        
        
        program.setModelMatrix(player1->getModelMatrix());
        glBindTexture(GL_TEXTURE_2D, entitySpriteTextureID);
//        program.translateViewMatrix(-player1->getXPos(), -player1->getYPos(), 0);
        player1->drawFromSheetSprite();
        player1->drawBullets();
        program.setModelMatrix(player2->getModelMatrix());
        player2->drawFromSheetSprite();
        player2->drawBullets();
    
        SDL_GL_SwapWindow(window.getDispWindow());
        
    }
    SDL_Quit();
    
}


int main(int argc, char* argv[]){
    //    playSpaceInvaders();
    char * dir = getcwd(NULL, 0);
    printf("Current dir: %s", dir);
    playPlatformGame();
    
//    bullet
//    <SubTexture name="slice139_@.png" x="178" y="148" width="16" height="12"/>
    
}