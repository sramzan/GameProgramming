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
    
    void setPerspective(){
        projectionMatrix.setOrthoProjection(-4, 4, -2.0f, 2.0f, -1.0f, 1.0f);
    }
    
    void setMatrices(){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
        program->setViewMatrix(viewMatrix);
    }
    
    void setTexture(GLuint text, string textName){
        this->texture = text;
        this->textureName = textName;
    }
    
    void drawTexture(float* verticesArray, float* texCords){
//        if (texture == NULL)
//            throw new LoadException("Texture not defined for " + textureName + "\n");
        
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, texture); // whatever texture is bound, is the one that will be mapped to the textureCoordinates
        
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
    
    void moveToTheRight(float xPos){
        if (xPos >= 0){
            modelMatrix.Translate(xPos, 0, 0);
            program->setModelMatrix(modelMatrix); // MUST set the model matrix everytime a transformation is made as the program will not know what changes have been in the matrix, if not explicitly told. The matrices simply hold a bunch of values that can be used to plot points on a screen
        }
    }
    
    void moveToTheLeft(float xPos){
        if (xPos <= 0){
            modelMatrix.Translate(xPos, 0, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveUp(float yPos){
        if (yPos >= 0){
            modelMatrix.Translate(0, yPos, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void moveDown(float yPos){
        if (yPos <= 0){
            modelMatrix.Translate(0, yPos, 0);
            program->setModelMatrix(modelMatrix);
        }
    }
    
    void move(float xPos, float yPos, float zPos){ // General move function to move the screen obj around on the screen
        modelMatrix.Translate(xPos, yPos, zPos);
        program->setModelMatrix(modelMatrix);
    }
    
    void drawPolygon(){
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesArray);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);

    }
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    GLuint texture = NULL;
    string textureName = "";
    ShaderProgram* program;
    
    // Values will be used to set the prospective per Animation
};