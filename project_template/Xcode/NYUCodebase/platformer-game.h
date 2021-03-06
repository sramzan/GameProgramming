//
//  platformer-game.h
//  
//
//  Created by Sean Ramzan on 3/25/16.
//
//

#ifndef ____platformer_game__
#define ____platformer_game__

#include <vector>
#include "screenObject.cpp"
#include <map>
#include <SDL_mixer.h>

//using namespace std;

/*
    * Assignment:
    *   - Make a simple scrolling platformer game demo
    *   - It must use a tilemap or static/dynamic entities
    *   - It must scroll
    *   - It must be either procedurally generated or load levels from a file
*/

//int** levelData;

// global variables

// functions
void getVertexAndTextureCoordsFromTileMap();
void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
void readFileLineByLine();
void placeEntity(std::string type, float placeX, float placeY);
void playPlatformGame();
void getVertexAndTextureDataAtPoint(int x, int y);

bool readHeader(std::ifstream &stream);
bool readLayerData(std::ifstream &stream);
bool readEntityData(std::ifstream &stream);

GLuint LoadTexture(const char* image_path);

class Bullet;
class Entity;
class TileMap;

// Classes
class Entity : public ScreenObject{
public:
    Entity(ShaderProgram* prog, string entityType, int x, int y, float uVal, float vVal, float widthVal, float heightVal,float sizeVal, int direction, int healthVal);
    std::string getType();
    bool switchSprite();
    int getDirection();
    int getHealth();
    float getXPos();
    float getYPos();
    float getX_Velocity();
    float getY_Velocity();
    float getX_Acceleration();
    float getY_Acceleration();
    float getSize();
    void applyGravityToVel(float elapsed);
    void applyAccelerationToVel();
    void updatePosition();
    void applyFrictionToVel();
    void updateXPos(float xVel);
    void updateYPos(float yVel);
    void setY_Velocity(float vel);
    void setX_Velocity(float vel);
    void draw();
    void getVertexAndTextCoordData();
    void fire(Mix_Chunk* energyBlastSound);
    void setDirectionLeft();
    void setDirectionRight();
    void moveBullets();
    void deleteBullet(Bullet* bullet);
    void checkBulletCollisions(float elapsed);
    void drawBullets();
    void setBulletsToIdentity();
    void moveAllBulletsToPosition();
    void applyAccelerationToVelForAllBullets();
    void updateAllBulletPositions();
    void applyAccelerationToVelFor(Bullet* bullet);
    void invertBulletsIfNecessary();
    void checkBulletCollisions(Entity* player, float elapsed);
    void decrementHealth();
    void deleteAllBullets();
    void resetHealth();
    void switchSpriteNextTime();
    void changeSpriteCoords(float uVal, float vVal, float width, float height);
    
    // Setter Function
    void updateVelocityTo(float velX, float velY);
    void updateAccelTo(float accelX,  float accelY);
    bool isOnFloor;
private:
    std::string type;
    int direction;
    int health;
    float xVelocity;
    float yVelocity;
    float xAccel;
    float yAccel;
    float xPos;
    float yPos;
    float size;
    float u;
    float v;
    float spriteWidth;
    float spriteHeight;
    bool shouldSwitchSprite;
    std::vector<Bullet*> bullets;
};

class Bullet : public Entity{
public:
    Bullet(ShaderProgram* prog, float xPos, float yPos, int direction);
    void markForDeletion();
    bool isMarkedForDeletion();
    int getBulletDirection();
    float getInitialX();
    float getInitialY();
    
private:
    int bulletDirection;
    bool markedForDeletion;
    float initialXPos;
    float initialYPos;
    
};

class TileMap : public ScreenObject{
public:
    TileMap(ShaderProgram* prog);
    float* getVertexData();
    float* getTextData();
    vector<float>* getVertexVector();
    vector<float>* getTextVector();
    bool isSolidTile(float xLoc, float yLoc);
private:
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
};

int getSpriteSheetValof(Entity* entity);

// Global Data Structures
std::vector<float> entityVertexData;
std::vector<float> entityTextureData;
std::vector<Entity*> entities;


typedef std::map<std::string, int> playerMapping;


#endif /* defined(____platformer_game__) */
