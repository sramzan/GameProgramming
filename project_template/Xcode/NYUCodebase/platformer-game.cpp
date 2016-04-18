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

#define SPRITE_COUNT_X 30 // check this
#define SPRITE_COUNT_Y 30 // check this
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define LEVEL_HEIGHT 30
#define LEVEL_WIDTH 30
#define GRAVITY_X 0
#define GRAVITY_Y 0
#define FRICTION_X .99
#define FRICTION_Y .99
#define SPRITE_SIZE 21

static ShaderProgram* prog;
static GLuint spriteText;
float TILE_SIZE=1.25;
float tileMidBaseLine   = 0;
int mapWidth    = -1;
int mapHeight   = -1;
int solidGround = 586;
int** levelData;
int** solidTiles;
bool firstRun = true;

std::string levelFilePath = "level1.txt";
std::string tileLayerName = "layer";
std::string objLayerName  = "ObjLayer";

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

Entity::Entity(ShaderProgram* prog, string entityType, int xPosition, int yPosition) : ScreenObject(prog, spriteText){
    if (entities.size() > 0){
        delete entities[0];
        entities.pop_back();
    }
    this->type=entityType;
    
    // Set position of entity in space
    this->xPos = (float) (xPosition)/SPRITE_SIZE;
    this->yPos = (float) (yPosition)/-SPRITE_SIZE;
    
    // default velocity
    this->xVelocity = 0;
    this->yVelocity = 0;
    
    // default acceleration
    this->xAccel = 0;
    this->yAccel = 0;
    
    setPositionVars((float)xPosition /SPRITE_SIZE, (float)yPosition/SPRITE_SIZE, 0, TILE_SIZE);
    
//    translateViewMatrix(8, -14, 0);
//    setPosition(x, y, 0, TILE_SIZE);
}

// Entity Class Definitions
string Entity::getType()          { return this->type;      }
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
    this->xPos += xVelocity * FIXED_TIMESTEP;
    this->yPos += yVelocity * FIXED_TIMESTEP;
}

void Entity::updateAccelTo(float accelX, float accelY){
    this->xAccel = accelX;
    this->yAccel = accelY;
}

// TileMap Class Definitions
TileMap::TileMap(ShaderProgram* prog) : ScreenObject(prog, spriteText){}
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
    glDrawArrays(GL_TRIANGLES, 0, LEVEL_HEIGHT * LEVEL_WIDTH * 6);
    
    glDisableVertexAttribArray(prog->positionAttribute);
    glDisableVertexAttribArray(prog->texCoordAttribute);
}

int getSpriteSheetValof(Entity* entity){
    static playerMapping players;
    players["pinkPlayer"] = 57;
    return players[entity->getType()];
}

bool isSolidGround(int val){
    return (val == solidGround) ? true : false;
}

void getVertexAndTextureDataAtPoint(Entity* entity, float x, float y){
//        float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
//        float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
        int playerSpritesheetVal = getSpriteSheetValof(entity);
        float u = (float) (( playerSpritesheetVal % SPRITE_COUNT_X )/ (float) SPRITE_COUNT_X);
        float v = ((float)(playerSpritesheetVal)/ SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;

        
        
        float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
        float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
//    cout << "u: " << u << " v: " << v << " Val: " << levelData[y][x] << " Sprite Width: " << spriteWidth << " Sprite Height: " << spriteHeight << endl << endl;
    // set left, right, bottom, top boundaries of each respective obj
    float size = TILE_SIZE;
    float aspect = spriteWidth / spriteHeight;
//    entity-> leftPos   = -0.5f * size * aspect;
//    entity-> rightPos  =  0.5f * size * aspect;
//    entity-> bottomPos = -0.5f * size;
//    entity-> topPos    =  0.5f * size;
        entityVertexData.insert(entityVertexData.end(), {
            TILE_SIZE * x,  -TILE_SIZE * y,
            TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
            (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
            TILE_SIZE * x, -TILE_SIZE * y,
            (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
            (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
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

void getVertexAndTextureCoordsFromTileMap(TileMap* tm){
    vector<float>* tilemapVertexData = tm->getVertexVector();
    vector<float>* tilemapTextData   = tm->getTextVector();
    for(int y=0; y < LEVEL_HEIGHT; y++) {
        for(int x=0; x < LEVEL_WIDTH; x++) {
            if(levelData[y][x] != 0){
                if (isSolidGround(levelData[y][x]) && tileMidBaseLine == 0){
                    tileMidBaseLine = y - (TILE_SIZE / 2);
                }
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
        }
    }
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) { // way to get entities' grid location in space
    *gridX = (int)(worldX * SPRITE_SIZE);
    *gridY = (int)(-worldY * SPRITE_SIZE);
}

void placeEntity(string type, float placeX, float placeY){
    Entity* newEntity = new Entity(prog, type.c_str(), placeX, placeY);
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
            float placeX = 6; //* TILE_SIZE; //atoi(xPosition.c_str()); // NOT GETTING PROPER INDEX... FIXME
            float placeY = 12; //* -TILE_SIZE; //atoi(yPosition.c_str());
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

void checkForCollision(Entity* entity, float elapsed){
    int xLoc, yLoc;
    float top    = entity->getTopPos();
    float bottom = entity->getBottomPos();
    float left   = entity->getLeftPos();
    float right  = entity->getRightPos();
    float middle = (top+bottom)/2;
    middle = (middle < 0) ? middle : middle * -1;
    // Checking y
    
    // check left
    worldToTileCoordinates(fabs(left), middle, &xLoc, &yLoc);
    if (levelData[yLoc][xLoc] == solidGround){
        cout << "LEFT collision" << endl;
    }
    
    // check right
    worldToTileCoordinates(fabs(right), middle, &xLoc, &yLoc);
    if (levelData[yLoc][xLoc] == solidGround){
        cout << "RIGHT collision" << endl;
    }
    
    
    if (levelData[yLoc][xLoc] == solidGround){
        //move the entity back up
        
//        float y_distance  = fabs((entity->getTopPos() - (TILE_SIZE/2))) + fabs(tileMidBaseLine);
//        float height1     = fabs((entity->getTopPos() - (TILE_SIZE/2)));
//        float height2     = tileMidBaseLine;
//        float penetration = fabs(y_distance - height1/2 - height2/2) + .000001;
//        entity->moveUp(penetration);
    }
}
void doTheEntityThing(Entity* entity, const Uint8* keys, float elapsed, Program* program){
//    if (entities.size() > 0){
//        Entity* entity = entities[0];
//        entity->setModelMatrix();
    
        // Apply Gravity
        if (entity == NULL)
            return;
        
        if (firstRun){
            getVertexAndTextureDataAtPoint(entity, entity->getXPos(), entity->getYPos());
            firstRun = false;
        }
        if (keys[SDL_SCANCODE_RIGHT]){
            entity->updateAccelTo(.2, 0);
            entity->applyAccelerationToVel();
        }
        if (keys[SDL_SCANCODE_LEFT]){
            entity->updateAccelTo(-.1, 0);
            entity->applyAccelerationToVel();
        }
        
        if (keys[SDL_SCANCODE_SPACE]){
            entity->updateAccelTo(0, .1);
            entity->applyAccelerationToVel();
        }
        int xPos, yPos;
        
        entity->applyFrictionToVel();
        entity->applyGravityToVel(elapsed);
        entity->move(entity->getX_Velocity(), entity->getY_Velocity(), 0);
    
        entity->updatePosition();
    
        checkForCollision(entity, elapsed);
        entity->updateAccelTo(0, 0);
    
        
        worldToTileCoordinates(entities[0]->getXPos(), entities[0]->getYPos(), &xPos, &yPos);
        getVertexAndTextureDataAtPoint(entity, 6, 12);
        entities[0]->drawTexture(entityVertexData.data(), entityTextureData.data());
        
    cout << "top : " << entity->getTopPos() << " bottom: " << entity->getBottomPos() << " left: " << entity->getLeftPos() << " right: " << entity->getRightPos() << " x: " << entity->getXPos() << " y: " << entity->getYPos() << " xVel: " << entity->getX_Velocity() << " yVel: " << entity->getY_Velocity() << " tileX: " << xPos << " tileY: " << yPos << endl;
        
        entityVertexData.clear();
        entityTextureData.clear();
    
}
void playPlatformGame(){
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "The Awesome Platform Game - SR");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    prog = program.getShaderProgram();
    
    SDL_Event event;
    spriteText = LoadTexture("spritesheet.png");
    
    bool done     = false;
//    bool firstRun = true;
    float lastFrameTicks = 0.0f;
    bool getEntity = true;
    
    TileMap* tm = new TileMap(prog);
    readFileLineByLine();
    getVertexAndTextureCoordsFromTileMap(tm);
    Entity* entity = NULL;
    
    program.translateViewMatrix(tm->getLeftBoundary() * 1.10, tm->getTopBoundary() * 2, 0);
    while (!done) {
        float ticks    = (float)SDL_GetTicks()/1000.0f;
        float elapsed  = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        program.clearScreen(.2, .3, .4, 1);
        
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        tm->identity();
        drawTexture(tm->getVertexData(), tm->getTextData());
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        
        if (entities.size() > 0){
            if(getEntity)
                entity = entities[0];
        
            while (fixedElapsed >= FIXED_TIMESTEP ) {
                fixedElapsed -= FIXED_TIMESTEP;
//              updateGame(program, player1, window, FIXED_TIMESTEP, keys);
                doTheEntityThing(entity, keys, fixedElapsed, &program);
            
            }
            doTheEntityThing(entity, keys, fixedElapsed, &program);
            getEntity = false;
        }
//        updateGame(program, player1, window, fixedElapsed, keys);


//        float fixedElapsed = elapsed;
        SDL_GL_SwapWindow(window.getDispWindow());
        
    }
    SDL_Quit();
    
}


int main(int argc, char* argv[]){
//    playSpaceInvaders();
    char * dir = getcwd(NULL, 0);
    printf("Current dir: %s", dir);
    playPlatformGame();
    
    
    
}