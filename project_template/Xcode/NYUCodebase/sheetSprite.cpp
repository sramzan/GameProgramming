//
//  sheetSprite.cpp
//  
//
//  Created by Sean Ramzan on 2/27/16.
//
//

#include <stdio.h>


class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textureID, float u, float v, float width, float height,float size);

    float size;
    float u;
    float v;
    float width;
    float height;
    
    unsigned int textureID;
    
    void Draw();
};

void SheetSprite::Draw() {
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
}

