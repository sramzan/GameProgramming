//
//  sheetSprite.cpp
//  
//
//  Created by Sean Ramzan on 2/27/16.
//
//

#include <stdio.h>
#include "ShaderProgram.h"
#ifdef _WINDOWS
#include "screenObject.cpp"
#endif

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
    
    void setSpriteCoords(float uVal, float vVal, float widthVal, float heightVal){
        this->u = uVal;
        this->v = vVal;
        this->width  = widthVal;
        this->height = heightVal;
    }
    
    
    
    float getTopPos()    { return topPos;    }
    float getBottomPos() { return bottomPos; }
    float getRightPos()  { return rightPos;  }
    float getLeftPos()   { return leftPos;   }
    float getSize()      { return size;      }
    float getUCoord()    { return u;         }
    float getVCoord()    { return v;         }
    float getWidth()     { return width;     }
    float getHeight()    { return height;    }
    float getTextID()    { return textureID; }
    
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
    
    unsigned int textureID;
};

