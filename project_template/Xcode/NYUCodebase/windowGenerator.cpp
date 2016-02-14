//
//  windowCreator.cpp
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


class Window{
    
public:
    
    Window(int height, int width, const char* nameOfGame){
        this->WINDOW_HEIGHT = height;
        this->WINDOW_WIDTH  = width;
        this->nameOfGame    = nameOfGame;
        createWindow();
    }
    
    void createWindow(){
        SDL_Init(SDL_INIT_VIDEO);
        displayWindow = SDL_CreateWindow(nameOfGame, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
        SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
        SDL_GL_MakeCurrent(displayWindow, context);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
private:
    int WINDOW_HEIGHT;
    int WINDOW_WIDTH;
    
    const char* nameOfGame;
    
    SDL_Window* displayWindow;
    
};
