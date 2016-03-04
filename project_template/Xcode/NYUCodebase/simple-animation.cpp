//
//  simple-animation.cpp
//  
//
//  Created by Sean Ramzan on 2/28/16.
//
//

#include <stdio.h>

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <math.h>
#include "../global-variables.cpp"
//#include "space-invaders.cpp"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

static SDL_Window* displayWindow;


void createWindow(const char* nameOfGame){
    SDL_Init(SDL_INIT_VIDEO); // initializes SDL
    displayWindow = SDL_CreateWindow(nameOfGame, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // will set the rendering area of the window. Tells OpenGL the rendering area, as it does not know how large the window is. 0,0 defines a starting position. You can actually have multiple viewports in one window (i.e. multiple views in an editing software)
}

//void setProgramID(ShaderProgram* program){
//    glUseProgram(program->programID);
//}

//void clearScreen(float red, float green, float blue, float alpha){
//    glClearColor(red, green, blue, alpha);
//    glClear(GL_COLOR_BUFFER_BIT);
//}

class SimpleAnimation{
public:
    SimpleAnimation(ShaderProgram* prog, GLuint text){
        this->texture = text;
        this->program = prog;
    }
    
    void setPerspective(){
        projectionMatrix.setOrthoProjection(-4, 4, -2.0f, 2.0f, -1.0f, 1.0f); /// will look the same anywhere as the viewport sets the amount of pixels. This sets a projection on the screen for where images can be drawn (check this)
    }
    
    void setMatrices(){
        program->setModelMatrix(modelMatrix);
        program->setProjectionMatrix(projectionMatrix);
        program->setViewMatrix(viewMatrix);
    }
    
    void drawTexture(float* verticesArray, float* texCords){
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, texture);
        
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
    }
    
    void moveToTheRight(float xPos){
        modelMatrix.Translate(xPos, 0, 0);
    }
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    GLuint texture;
    ShaderProgram* program;
};
