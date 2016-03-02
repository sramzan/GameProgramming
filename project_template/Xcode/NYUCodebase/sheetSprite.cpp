//
//  sheetSprite.cpp
//  
//
//  Created by Sean Ramzan on 2/27/16.
//
//

#include <stdio.h>
#include "ShaderProgram.h"
#include "../screenObject.cpp"


class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textID, float uVal, float vVal, float widthVal, float heightVal,float sizeVal){
        this->textureID = textID;
        this->u = uVal;
        this->v = vVal;
        this-> width = widthVal;
        this->height = heightVal;
        this->size = sizeVal;
        
        float aspect = width / height;
        
        // set left, right, bottom, top boundaries of each respective obj
        this-> leftPos   = -0.5f * size * aspect;
        this-> rightPos  =  0.5f * size * aspect;
        this-> bottomPos = -0.5f * size;
        this-> topPos    =  0.5f * size;
         
    }
    
    unsigned int textureID;
    
    void Draw(ScreenObject* obj) {
        glBindTexture(GL_TEXTURE_2D, textureID);
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
             0.5f * size * aspect,  0.5f * size,
            -0.5f * size * aspect,  0.5f * size,
            
             0.5f * size * aspect,  0.5f * size,
            -0.5f * size * aspect, -0.5f * size,
             0.5f * size * aspect, -0.5f * size
        };
        // draw our arrays
        obj->drawTexture(vertices, texCoords);
    }
    
    float getTopPos()    { return topPos;    }
    float getBottomPos() { return bottomPos; }
    float getRightPos()  { return rightPos;  }
    float getLeftPos()   { return leftPos;   }
    
    
private:
    float size;
    float u;
    float v;
    float width;
    float height;
    
    float topPos;
    float bottomPos;
    float leftPos;
    float rightPos;
};

