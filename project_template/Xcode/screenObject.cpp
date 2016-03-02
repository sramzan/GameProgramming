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
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <stdio.h>
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
    }
    
    void setProjection(){
        // setOrthoProjection(      float left, float right, float bottom, float top, float near, float far);
        projectionMatrix.setOrthoProjection(LEFT_BOUNDARY, RIGHT_BOUNDARY, BOTTOM_BOUNDARY, TOP_BOUNDARY, NEAR_BOUNDARY, FAR_BOUNDARY);
//        projectionMatrix.setOrthoProjection(-4, 4, -2.0f, 2.0f, -1.0f, 1.0f);
    }
    
    void setMatrices(){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
        program->setViewMatrix(viewMatrix);
    }
    
    void setModelMatrix(){
        program->setModelMatrix(modelMatrix);
    }
    
    void setTexture(GLuint text, string textName){
        this->texture = text;
        this->textureName = textName;
    }
    
    void drawTexture(float* verticesArray, float* texCords){
//        if (texture == NULL)
//            throw new LoadException("Texture not defined for " + textureName + "\n");
        
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
    
    void rotate(float rotateAmt){
        modelMatrix.Rotate(rotateAmt);
        program->setModelMatrix(modelMatrix);
    }
    
    void moveToTheRight(float xUnits){
        if (xUnits >= 0){
            leftPos  += xUnits;
            rightPos += xUnits;
            modelMatrix.Translate(xUnits, 0, 0);
            program->setModelMatrix(modelMatrix); // MUST set the model matrix everytime a transformation is made as the program will not know what changes have been in the matrix, if not explicitly told. The matrices simply hold a bunch of values that can be used to plot points on a screen
        }
    }
    
    void moveToTheLeft(float xUnits){
        if (xUnits <= 0){
            leftPos += xUnits;
            rightPos += xUnits;
            modelMatrix.Translate(xUnits, 0, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveUp(float yUnits){
        if (yUnits >= 0){
            topPos += yUnits;
            bottomPos += yUnits;
            modelMatrix.Translate(0, yUnits, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveDown(float yUnits){
        if (yUnits <= 0){
            topPos += yUnits;
            bottomPos += yUnits;
            modelMatrix.Translate(0, yUnits, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void move(float xUnits, float yUnits, float zUnits){ // General move function to move the screen obj around on the screen
        topPos    += yUnits;
        bottomPos += yUnits;
        leftPos   += xUnits;
        rightPos  += xUnits;
        modelMatrix.Translate(xUnits, yUnits, zUnits);
        program->setModelMatrix(modelMatrix);
    }
    
    void setPosition(float top, float bottom, float left, float right){ // General move function to move the screen obj around on the screen
        topPos    = top;
        bottomPos = bottom;
        leftPos   = left;
        rightPos  = right;
//        modelMatrix.setPosition(, yUnits, zUnits);
//        program->setModelMatrix(modelMatrix);
    }
    
    void setPosition(float xUnits, float yUnits, float zUnits){
        topPos    += yUnits;
        bottomPos += yUnits;
        leftPos   += xUnits;
        rightPos  += xUnits;
        modelMatrix.setPosition(xUnits, yUnits, zUnits);
        program->setModelMatrix(modelMatrix);
    }
    
    
    
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
        program->setModelMatrix(modelMatrix);
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
    
    void destroy(){
        
    }
    
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
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
    

    
    // Values will be used to set the prospective per Animation
};