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
    
private:
    ShaderProgram program;
    
};






