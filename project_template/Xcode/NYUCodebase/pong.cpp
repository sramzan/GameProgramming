//
//  Pong.cpp
//  
//
//  Created by Sean Ramzan on 2/14/16.
//
//

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>

#include "Matrix.h"
#include "ShaderProgram.h"

#include "../programGenerator.cpp"
#include "windowGenerator.cpp"
#include "../screenObject.cpp"

using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

int WINDOW_HEIGHT = 900;
int WINDOW_WIDTH  = 1500;

SDL_Window* displayWindow;

float vertices_blockA[] =
{
    -5.0,  2.0,
    -5.0, -2.0,
    -4.0,  2.0,
    
    -5.0, -2.0,
    -4.0, -2.0,
    -4.0,  2.0
};

float vertices_blockB[] =
{
    4.0, 2.0,
    4.0, -2.0,
    5.0, 2.0,
    
    4.0, -2.0,
    5.0, -2.0,
    5.0, 2.0
};

float vertices_ball[] =
{
    -0.5,   0.5,
    -0.5,  -0.5,
     0.5,   0.5,
    
    -0.5,  -0.5,
     0.5,  -0.5,
     0.5,   0.5,
    
};

void setup(const char* windowTitle){
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, windowTitle);
    ScreenObject ball(program.getShaderProgram());
    
}

void runPong(){
//    setup("Pong");
    Program program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); // defines new shader program, and sets the program ID. This is the obj that will be used to clear the screen every frame
    
    Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "Pong");
    ScreenObject ball(program.getShaderProgram());
    ball.setMatrices();
    
    SDL_Event event;
    bool done = false;
    
    while (!done) {
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
}