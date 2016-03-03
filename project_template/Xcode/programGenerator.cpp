//
//  programGenerator.cpp
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
#include "NYUCodebase/exceptions.cpp"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


class Program{
public:
    Program(const char *vertexShaderFile, const char *fragmentShaderFile) : program(vertexShaderFile, fragmentShaderFile){
        setProgramID();
    }
    
    void setProgramID(){
        glUseProgram(program.programID);
    }
    
    
    void clearScreen(float red, float green, float blue, float alpha){
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    ShaderProgram* getShaderProgram(){
        return &program;
    }
    
    float getTopBoundary()    {  return TOP_BOUNDARY;    }
    
    float getBottomBoundary() {  return BOTTOM_BOUNDARY; }
    
    float getLeftBoundary()   {  return LEFT_BOUNDARY;   }
    
    float getRightBoundary()  {  return RIGHT_BOUNDARY;  }
    
private:
    ShaderProgram program;
    
    // TODO - have all objects look here for boundary constraints as the program should know about the ortho boundaries, and each object should have the ability to overwrite its personal boundaries from there
    
    float LEFT_BOUNDARY   = -14.0;
    float RIGHT_BOUNDARY  =  14.0;
    float BOTTOM_BOUNDARY = -8.0;
    float TOP_BOUNDARY    =  8.0;
    float NEAR_BOUNDARY   = -4.0;
    float FAR_BOUNDARY    =  4.0;
    
};







