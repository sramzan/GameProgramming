//
//  animation.cpp
//  
//
//  Created by Sean Ramzan on 2/11/16.
//
//

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <stdio.h>
#include "sheetSprite.cpp"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class ScreenObject{
public:
    ScreenObject(ShaderProgram* prog){
        this->program = prog;
//        setProjection();
//        setMatrices();
    }
    
    ScreenObject(ShaderProgram* prog, GLuint texture){
        this->program = prog;
        this->spriteSheetTexture = texture;
//        setProjection();
//        setMatrices();
    }
    
    // used for adding a sheetSprite to the respective entity
    ScreenObject(ShaderProgram* prog, unsigned int textID, float uVal, float vVal, float widthVal, float heightVal,float sizeVal):
        program(prog),
        sheetSprite(new SheetSprite(textID, uVal, vVal, widthVal, heightVal, sizeVal)){
            setPositionInSpace(sheetSprite->getTopPos(), sheetSprite->getBottomPos(), sheetSprite->getLeftPos(), sheetSprite->getRightPos());
    }
    void setProjection(){
        projectionMatrix.setOrthoProjection(LEFT_BOUNDARY, RIGHT_BOUNDARY, BOTTOM_BOUNDARY, TOP_BOUNDARY, NEAR_BOUNDARY, FAR_BOUNDARY);
    }
    
    void setMatrices(){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
//        program->setViewMatrix(viewMatrix);
    }
    
    void setModelMatrix(){
        program->setModelMatrix(modelMatrix);
    }
    
    void setTexture(GLuint text, string textName){
        this->texture = text;
        this->textureName = textName;
    }
    
    void createNewSprite(unsigned int textID, float uVal, float vVal, float widthVal, float heightVal,float sizeVal){
        this->sheetSprite = new SheetSprite(textID, uVal, vVal, widthVal, heightVal, sizeVal);
    }
    
    void drawTexture(float* verticesArray, float* texCords){
//        if (texture == NULL)
//            throw new LoadException("Texture not defined for " + textureName + "\n");
        glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
        glEnable(GL_BLEND);
//        glBindTexture(GL_TEXTURE_2D, texture); // whatever texture is bound, is the one that will be mapped to the textureCoordinates
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesArray);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    }
    
    void drawFromSheetSprite() {
        float u = sheetSprite->getUCoord();
        float v = sheetSprite->getVCoord();
        float width = sheetSprite->getWidth();
        float height = sheetSprite->getHeight();
        float size = sheetSprite->getSize();

        glBindTexture(GL_TEXTURE_2D, sheetSprite->getTextID());
        GLfloat texCoords[] = {
            u, v+height,
            u+width, v,
            u, v,
            u+width, v,
            u, v+height,
            u+width, v+height
        };
        float aspect = width / height;
        float vertices[] = {
            -0.5f * size * aspect, -0.5f * size,
             0.5f * size * aspect,   0.5f * size,
            -0.5f * size * aspect,  0.5f * size,
            
             0.5f * size * aspect,  0.5f * size,
            -0.5f * size * aspect, -0.5f * size,
             0.5f * size * aspect, -0.5f * size
        };
        // draw our arrays
        drawTexture(vertices, texCoords);
    }
    
    void rotate(float rotateAmt){
        modelMatrix.Rotate(rotateAmt);
//        program->setModelMatrix(modelMatrix);
    }
    
    void moveToTheRight(float xUnits){
        if (xUnits >= 0){
            leftPos  += xUnits;
            rightPos += xUnits;
            modelMatrix.Translate(xUnits, 0, 0);
//            program->setModelMatrix(modelMatrix); // MUST set the model matrix everytime a transformation is made as the program will not know what changes have been in the matrix, if not explicitly told. The matrices simply hold a bunch of values that can be used to plot points on a screen
        }
    }
    
    /*
    (x/TileSize)
    (y/-TileSize)
        - y goes down in openGL, thats why its negative
    what worlToTile does to get you the pointsin the TileMap
    */
     
     void moveToTheLeft(float xUnits){
        if (xUnits <= 0){
            leftPos += xUnits;
            rightPos += xUnits;
            modelMatrix.Translate(xUnits, 0, 0);
//            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveUp(float yUnits){
        if (yUnits >= 0){
            topPos += yUnits;
            bottomPos += yUnits;
            modelMatrix.Translate(0, yUnits, 0);
//            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveDown(float yUnits){
        if (yUnits <= 0){
            topPos += yUnits;
            bottomPos += yUnits;
            modelMatrix.Translate(0, yUnits, 0);
//            program->setModelMatrix(modelMatrix);
        }
    }
    
    void move(float xUnits, float yUnits, float zUnits){ // General move function to move the screen obj around on the screen... used fixedTimeStep here at a point
        topPos    += yUnits;
        bottomPos += yUnits;
        leftPos   += xUnits;
        rightPos  += xUnits;
        modelMatrix.Translate(xUnits, yUnits, zUnits);
//        program->setModelMatrix(modelMatrix);
    }
    
    void setPositionInSpace(float top, float bottom, float left, float right){ // General move function to move the screen obj around on the screen
        topPos    = top;
        bottomPos = bottom;
        leftPos   = left;
        rightPos  = right;
//        modelMatrix.setPosition(, yUnits, zUnits);
//        program->setModelMatrix(modelMatrix);
    }
    
//    void setPosition(float xUnits, float yUnits, float zUnits, float size){ // TODO - REMOVE
//        topPos    = yUnits;
//        bottomPos = yUnits - size;
//        leftPos   = xUnits;
//        rightPos  = xUnits + size;
//        modelMatrix.setPosition(xUnits, yUnits, zUnits);
//        program->setModelMatrix(modelMatrix);
//    }
    
    void setPosition(float xUnits, float yUnits, float zUnits, float size){
        topPos    = yUnits + size;
        bottomPos = yUnits - size;
        leftPos   = xUnits - size;
        rightPos  = xUnits + size;
        modelMatrix.setPosition(xUnits, yUnits, zUnits);
//        program->setModelMatrix(modelMatrix);
    }
    
    void setPositionVars(float xUnits, float yUnits, float zUnits, float size){
        topPos    = yUnits + size/2;
        bottomPos = yUnits - size/2;
        leftPos   = xUnits - size/2;
        rightPos  = xUnits + size/2;
    }
    
//    void translateViewMatrix(float x, float y, float z){
//        viewMatrix.Translate(x, y, z);
//        program->setViewMatrix(viewMatrix);
//    }
    
    
    void drawPolygon(float* vertices){
//        void glVertexAttribPointer (GLint index, GLint
//                                    size, GLenum type, GLboolean normalized, GLsizei
//                                    stride, const GLvoid *pointer);
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);

    }
    
    void resetPosition(){
        modelMatrix.identity();
//        program->setModelMatrix(modelMatrix);
    }
    
    void setTopPos(float top){
        topPos = top;
    }
    
    void setBottomPos(float bottom){
        bottomPos = bottom;
    }
    
    void setLeftPos(float left){
        leftPos = left;
    }
    
    void setRightPos(float right)
    {
        rightPos = right;
    }
    
    /*
    void setBoundaries(float top, float bottom, float left, float right){
        topPos    = top;
        bottomPos = bottom;
        leftPos   = left;
        rightPos  = right;
    }
    */
    
    
    float getTopPos(){
        return topPos;
    }
    
    float getBottomPos(){
        return bottomPos;
    }
    
    float getLeftPos(){
        return leftPos;
    }
    
    float getRightPos(){
        return rightPos;
    }
    
    float getTopBoundary(){
        return TOP_BOUNDARY;
    }
    
    float getBottomBoundary(){
        return BOTTOM_BOUNDARY;
    }
    
    float getLeftBoundary(){
        return LEFT_BOUNDARY;
    }
    
    float getRightBoundary(){
        return RIGHT_BOUNDARY;
    }
    
    void identity(){
        modelMatrix.identity();
//        program->setModelMatrix(modelMatrix);
    }
    
    void scale(float x, float y, float z){
        modelMatrix.Scale(x, y, z);
    }
    
    void destroy(){
        
    }
    
    Matrix getModelMatrix() { return modelMatrix; }
    
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
//    Matrix viewMatrix;
    GLuint texture = NULL;
    string textureName = "";
    ShaderProgram* program;
    
    // these vars are used to set the ortho projection
    float LEFT_BOUNDARY   = -14.0;
    float RIGHT_BOUNDARY  =  14.0;
    float BOTTOM_BOUNDARY = -8.0;
    float TOP_BOUNDARY    =  8.0;
    float NEAR_BOUNDARY   = -4.0;
    float FAR_BOUNDARY    =  4.0;
    
    // these vars are used to keep track of where the objects are located in space
    float topPos    = 0.0f;
    float bottomPos = 0.0f;
    float leftPos   = 0.0f;
    float rightPos  = 0.0f;
    
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    bool isStatic;
//    EntityType entityType;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    GLuint spriteSheetTexture;
    
protected:
        SheetSprite* sheetSprite;

    
    // Values will be used to set the prospective per Animation
};